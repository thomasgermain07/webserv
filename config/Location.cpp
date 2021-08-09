/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/01 19:00:13 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 19:01:03 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location()
{
	this->client_max_body_size = 0;
	this->authenticate.active = false;
	this->auto_index = false;
}