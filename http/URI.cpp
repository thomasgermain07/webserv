/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/06 15:30:58 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/08 00:22:26 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger/Console.hpp"
#include "URI.hpp"

/**
 * Static const
 */
const std::string URI::SEP_PROTOCOL	= "://";
const std::string URI::SEP_HOST		= "/";
const std::string URI::SEP_PORT		= ":";
const std::string URI::SEP_PATH		= "?";
const std::string URI::SEP_QUERY	= "#";

/**
 * Constructors & Destructor
 */
URI::URI(
	std::string raw
)
{
	std::string::size_type found;

	// Console::log("URI : " + raw);

	// Protocol
	found = raw.find(SEP_PROTOCOL);
	if (found != std::string::npos)
	{
		this->protocol = raw.substr(0, found);
		raw.erase(0, found + SEP_PROTOCOL.length());
	}
	// Console::log("protocol: `" + this->protocol + "`");

	// Host
	found = raw.find(SEP_HOST);
	if (found != 0)
	{
		this->host = raw.substr(0, found);
		raw.erase(0, found);

		// Port
		found = this->host.find(SEP_PORT);
		if (found != std::string::npos)
		{
			this->port = this->host.substr(found + SEP_PORT.length());
			this->host.erase(found);
		}
	}
	// Console::log("host: `" + this->host + "`");
	// Console::log("port: `" + this->port + "`");

	// Path
	found = raw.find(SEP_PATH);
	this->path = raw.substr(0, found);
	raw.erase(0, found + SEP_PATH.length());
	// Console::log("path: `" + this->path + "`");

	// Query
	found = raw.find(SEP_QUERY);
	this->query = raw.substr(0, found);
	raw.erase(0, found + SEP_QUERY.length());
	// Console::log("query: `" + this->query + "`");

	// Anchor
	this->anchor = raw;
	// Console::log("anchor: `" + this->anchor + "`");
}

/**
 * Getters & Setters
 */
std::string
URI::getHost() const
{
	std::string host = this->host;

	if (!this->port.empty())
		host += SEP_PORT + this->port;

	return host;
}


std::string
URI::getRootPath(
	const std::string & root
) const
{
	std::string root_path = root + "/" + this->path;
	std::string::size_type found;

	while ((found = root_path.find("//")) != std::string::npos)
		root_path.erase(found, 1);

	return root_path;
}
