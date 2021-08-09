/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/28 15:44:50 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/25 20:48:06 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include "logger/Console.hpp"
#include "Connection.hpp"
#include "Socket.hpp"
#include "ConnectionManager.hpp"

/**
 * Constructors & Destructor
 */
Socket::Socket(
	ConnectionManager & cm,
	const Config & cfg
)
	: fd_(-1)
	, cm_(cm)
	, cfg_(cfg)
{
	// Empty
}

Socket::~Socket()
{
	// Empty
}

/**
 * Methods
 */
void
Socket::start(
	in_addr_t addr,
	t_port port
)
{
	Console::log("Socket::start() : Socket is starting...", Console::INFO);

	// Create 'socket'
	if ((this->fd_ = socket(AF_INET , SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		throw std::runtime_error("Socket::start() : `socket()` failed.");
	}

	// Allow 'socket' to accept multiple connections
	// Should works without it, but it is a good practice
	int	enable = 1;
	if (setsockopt(this->fd_, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) < 0)
	{
		close(this->fd_);
		throw std::runtime_error("Socket::start() : `setsockopt()` failed.");
	}

	if (setsockopt(this->fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
	{
		close(this->fd_);
		throw std::runtime_error("Socket::start() : `setsockopt()` failed.");
	}

	// Set 'socket' as non-blocking (requested by subject)
	if (fcntl(this->fd_, F_SETFL, O_NONBLOCK) < 0)
	{
		close(this->fd_);
		throw std::runtime_error("Socket::start() : `fcntl()` failed.");
	}

	// Set 'socket' address informations
	sockaddr_in	socket_addr;

	memset(&socket_addr, 0, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(port);
	socket_addr.sin_addr.s_addr = addr;

	// Bind 'socket' to the address
	if (bind(this->fd_, (sockaddr*)&socket_addr, sizeof(socket_addr)) < 0)
	{
		close(this->fd_);
		throw std::runtime_error("Socket::start() : `bind()` failed.");
	}

	// Listen on 'socket'
	if (listen(this->fd_, SOMAXCONN) < 0)
	{
		close(this->fd_);
		throw std::runtime_error("Socket::start() : `listen()` failed.");
	}

	this->port_ = port;

	Console::log("Socket::start() : Socket started.", Console::INFO);
}

void
Socket::stop()
{
	// Close 'socket'
	close(this->fd_);

	Console::log("Socket::close() : Socket stopped.", Console::INFO);
}

t_fd
Socket::acceptConnection()
{
	t_fd		conn_fd;
	sockaddr_in	conn_addr;
	socklen_t	conn_addr_len = sizeof(conn_addr);

	// Accept new 'connection'
	if ((conn_fd = accept(this->fd_, (sockaddr*)&conn_addr, &conn_addr_len)) < 0)
	{
		throw std::runtime_error("Socket::acceptConnection() : `accept()` failed.");
	}

	// Set 'connection' as non-blocking (requested by subject)
	if (fcntl(conn_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(conn_fd);
		throw std::runtime_error("Socket::acceptConnection() : `fcntl()` failed.");
	}

	// Add 'connection' to 'connection manager'
	Connection * conn = new Connection(conn_fd, conn_addr, this->cfg_, *this);

	this->cm_.manage(conn);

	Console::log("Socket::acceptConnection() : New connection accepted.", Console::INFO);

	return conn_fd;
}

/**
 * Getters & Setters
 */
t_fd
Socket::getFD() const
{
	return this->fd_;
}

t_port
Socket::getPort() const
{
	return this->port_;
}