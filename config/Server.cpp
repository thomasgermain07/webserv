/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/14 14:57:05 by thgermai          #+#    #+#             */
/*   Updated: 2021/03/01 18:59:45 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Location.hpp"

Server::Server()
{
	this->host = 0;
	this->port = 0;
	this->client_max_body_size = 0;
	this->authenticate.active = false;
	this->auto_index = false;
}

Server::~Server()
{
	for (t_loc_ctn::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		delete it->second;
	}
}