/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/14 15:02:15 by thgermai          #+#    #+#             */
/*   Updated: 2021/02/18 19:17:37 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Server.hpp"

Config::~Config()
{
	typedef t_server_ctn::iterator			s_iterator;
	typedef t_srv_map_ctn::iterator			m_iterator;

	for (m_iterator m_it = srv_map.begin(); m_it != srv_map.end(); ++m_it)
	{
		t_server_ctn		srv_list = m_it->second;

		for (s_iterator s_it = srv_list.begin(); s_it != srv_list.end(); ++s_it)
			delete *s_it;
	}
}