/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/29 18:42:55 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 20:42:29 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include "logger/Console.hpp"
#include "utils/Time.hpp"
#include "utils/Network.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Connection.hpp"

/**
 * Static
 */
const size_t Connection::BUF_SIZE = 1024; // 1ko

/**
 * Constructor & Destructor
 */
Connection::Connection(
	t_fd socket_fd,
	sockaddr_in & socket_addr,
	const Config & cfg,
	const Socket & server
)
	: server_socket_(server)
	, socket_fd_(socket_fd)
	, socket_addr_(socket_addr)
	, interpreter_(cfg, server.getPort())
	, accepted_(true)
{
	this->updateTimeout();
}

Connection::~Connection()
{
	// Empty
}

/**
 * Methods
 */
void
Connection::do_read()
{
	char	buffer[BUF_SIZE + 1];
	ssize_t	read_length;

	if (!this->accepted_)
		return ;

	// Store incoming data into buffer
	if ((read_length = recv(this->socket_fd_, buffer, BUF_SIZE, MSG_DONTWAIT)) < 0)
	{
		return ;// throw std::runtime_error("Connection::do_read() : `recv()` failed.");
	}

	// Nothing received = disconnect
	if (read_length == 0)
	{
		// Set 'timeout' to 0 to force close by 'connection manager'
		this->timeout_ = 0;

		Console::log("Connection::do_read() : Disconnected.", Console::INFO);
	}
	else
	{
		buffer[read_length] = '\0';

		// Append read data to 'connection' buffer
		this->buffer_.append(buffer);

		// Update last activity timeout to avoid being closed by 'connection manager'
		this->updateTimeout();

		// Console::log("Connection::do_read() : Request data (buffer) received.", Console::INFO);

		// Parse 'buffer'
		if (!this->parser_.hasError())
		{
			this->parser_.parse(this->buffer_);
		}
	}
}

void
Connection::do_write()
{
	// No 'error' / 'request'
	if (!this->parser_.hasError() && !this->parser_.hasRequest() && this->accepted_)
		return ;

	Console::log("Connection::do_write() : Request received.", Console::INFO);

	// Request
	if (this->parser_.hasRequest())
	{
		Request * request = this->parser_.getRequest();

		// Handle 'request' to generate 'response'
		try
		{
			this->interpreter_.exec(*request, this);
		}
		catch(const std::exception& e)
		{
			Console::log(e.what());
		}

		delete request;

		// Update 'timeout' to avoid being closed by 'connection manager'
		this->updateTimeout();
	}
	// Parser Error
	else if (this->parser_.hasError())
	{
		Console::log("Connection::do_write() : Parser error.", Console::INFO);

		// Handle 'error'
		this->interpreter_.execError(this->parser_.getError(), false);
	}
	// Connection not 'accepted'
	else
	{
		Console::log("Connection::do_write() : Connection refused.", Console::INFO);

		// Handle 'error'
		this->interpreter_.execError(Response::SERVICE_UNAVAILABLE, false);
	}

	// A 'response' is ready
	std::string message;

	if (this->interpreter_.hasResponse())
	{
		Response * res = this->interpreter_.getResponse();

		if (res && res->headers["Connection"].find("close") != std::string::npos)
		{
			// Set 'timeout' to 0 to force close by 'connection manager'
			this->timeout_ = 0;
		}

		message = res->toString();
		delete res;
	}

	// Send 'response'
	size_t	remaining = message.length();
	ssize_t	done = 0;
	ssize_t sent = 0;

	while (remaining > 0)
	{
		if ((sent = send(this->socket_fd_, &message.c_str()[done], remaining, MSG_DONTWAIT)) > 0)
		{
			remaining -= (size_t)sent;
			done += sent;
		}
	}

	Console::log("Connection::do_write() : Response sent.", Console::INFO);
}

void
Connection::close()
{
	::close(this->socket_fd_);

	this->socket_fd_ = -1;

	Console::log("Connection::close() : Connection closed.", Console::INFO);
}

/**
 * Getters & Setters
 */
bool
Connection::isInactive() const
{
	return (this->timeout_ <= Time::getTime());
}

t_fd
Connection::getSocketFD() const
{
	return this->socket_fd_;
}


std::string
Connection::getIP() const
{
	return Network::addrToStr(this->socket_addr_.sin_addr.s_addr);
}

const Socket &
Connection::getServer() const
{
	return this->server_socket_;
}

void
Connection::setAccepted(
	bool accepted
)
{
	this->accepted_ = accepted;
}

/**
 * Private methods
 */
void
Connection::updateTimeout()
{
	this->timeout_ = Time::getTime() + 5 * 60; // 5min
}
