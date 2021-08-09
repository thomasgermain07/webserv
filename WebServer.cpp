/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/31 17:21:24 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 18:23:25 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "logger/Console.hpp"
#include "config/Server.hpp"
#include "utils/Network.hpp"
#include "WebServer.hpp"

/**
 * Static
 */
bool WebServer::running = false;

/**
 * Constructors & Destructor
 */
WebServer::WebServer(
	const Config & cfg
)
	: cfg_(cfg)
{
	// Empty
}

WebServer::~WebServer()
{
	// Empty
}

/**
 * Methods
 */
void
WebServer::run()
{
	try
	{
		this->start();
		this->loop();
		this->stop();
	}
	catch (const std::exception & e)
	{
		Console::log(e.what(), Console::ERROR);
	}
}

/**
 * Private methods
 */
void
WebServer::start()
{
	// Clear 'master set'
	FD_ZERO(&this->master_set_);

	// For each unique 'Server' port
	std::map<in_addr_t *, char*> used_host;
	for (Config::t_srv_map_ctn::const_iterator port_it = this->cfg_.srv_map.begin(); port_it != this->cfg_.srv_map.end(); ++port_it)
	{
		for (Config::t_server_ctn::const_iterator srv_it = port_it->second.begin(); srv_it != port_it->second.end(); ++srv_it)
		{
			Server * srv_ptr = *srv_it;

			if (used_host.count(&srv_ptr->host) == 1)
				continue ;

			// Create a new 'Socket'
			Socket * socket = new Socket(this->cm_, this->cfg_);

			try
			{
				socket->start(srv_ptr->host, port_it->first);

				// Add it to 'master set' for 'listen()'
				FD_SET(socket->getFD(), &this->master_set_);

				// Add it to 'Socket' container
				this->sockets_.push_back(socket);
			}
			catch (const std::exception & e)
			{
				Console::log(e.what(), Console::ERROR);
				delete socket;
			}
		}
	}
}

void
WebServer::loop()
{
	t_socket_ctn::const_iterator s;

	// Define 'fd_max' for 'select()'
	t_fd fd_max = 0;

	for (s = this->sockets_.begin(); s != this->sockets_.end(); ++s)
	{
		if ((*s)->getFD() > fd_max)
			fd_max = (*s)->getFD();
	}

	// Start 'infinite' loop
	// (Should be interruptable by signal)
	WebServer::running = true;
	while (WebServer::running)
	{
		// Create copies of 'master_set' because 'select()' modify them
		fd_set tmp_read_set = this->master_set_;
		fd_set tmp_write_set = this->master_set_;

		// Wait for communication
		// TOOD : Remove 'tmp_write_set' to avoid high cpu usage ?
		if (select(fd_max + 1, &tmp_read_set, &tmp_write_set, NULL, NULL) < 0)
		{
			std::runtime_error("WebServer::loop() : `select()` failed.");
		}

		// Event on 'Socket'
		for (s = this->sockets_.begin(); s != this->sockets_.end(); ++s)
		{
			if (FD_ISSET((*s)->getFD(), &tmp_read_set))
			{
				try
				{
					// Accept new 'connection'
					t_fd conn_fd = (*s)->acceptConnection();

					// Add it to 'master set'
					FD_SET(conn_fd, &this->master_set_);

					// Update 'fd max'
					if (conn_fd > fd_max)
						fd_max = conn_fd;
				}
				catch (const std::exception & e)
				{
					Console::log(e.what(), Console::ERROR);
				}
			}
		}

		// Event on 'Connection'
		ConnectionManager::t_connection_ctn::const_iterator c;

		for (c = this->cm_.getConnections().begin(); c != this->cm_.getConnections().end(); /* */)
		{
			// Received (Request)
			if (FD_ISSET((*c)->getSocketFD(), &tmp_read_set))
			{
				try
				{
					(*c)->do_read();
				}
				catch (const std::exception & e)
				{
					Console::log(e.what(), Console::ERROR);
				}
			}

			// Send (Response)
			if (FD_ISSET((*c)->getSocketFD(), &tmp_write_set))
			{
				try
				{
					(*c)->do_write();
				}
				catch (const std::exception & e)
				{
					Console::log(e.what(), Console::ERROR);
				}
			}

			// Activity
			if ((*c)->isInactive())
			{
				// Remove it from 'master set'
				FD_CLR((*c)->getSocketFD(), &this->master_set_);

				// Post-increment because we delete it while iterating though the container
				this->cm_.close(*c++);
			}
			else
			{
				// Default loop increment
				++c;
			}
		}
	}
}

void WebServer::stop()
{
	// Stop all 'connection'
	this->cm_.closeAll();

	// Stop all 'socket'
	for (t_socket_ctn::iterator s = this->sockets_.begin(); s != this->sockets_.end(); ++s)
	{
		(*s)->stop();
		delete *s;
	}

	Console::log("WebServer::stop() : Server stopped.", Console::INFO);

	// Clear 'master set'
	FD_ZERO(&this->master_set_);
}
