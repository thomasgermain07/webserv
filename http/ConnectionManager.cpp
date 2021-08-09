/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionManager.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/28 01:21:29 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/22 18:01:29 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionManager.hpp"

/**
 * Static const
 */
const int ConnectionManager::MAX_CONN = 256;

/**
 * Constructors & Destructor
 */
ConnectionManager::ConnectionManager()
{
	// Empty
}

ConnectionManager::~ConnectionManager()
{
	this->closeAll();
}

/**
 * Methods
 */
void
ConnectionManager::manage(
	Connection * connection
)
{
	if (this->connections_.size() >= MAX_CONN)
		connection->setAccepted(false);

	this->connections_.push_back(connection);
}

void
ConnectionManager::close(
	Connection * connection
)
{
	this->connections_.remove(connection);

	connection->close();
	delete connection;
}

void
ConnectionManager::closeAll()
{
	t_connection_ctn::iterator c = this->connections_.begin();

	while (c != this->connections_.end())
	{
		this->close(*c++);
	}
}

/**
 * Getters & Setters
 */
const ConnectionManager::t_connection_ctn &
ConnectionManager::getConnections() const
{
	return this->connections_;
}
