/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Network.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/07 19:02:31 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/25 22:01:58 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include "Converter.hpp"
#include "Network.hpp"

/**
 * Static
 */
std::string
Network::addrToStr(
	in_addr_t address
)
{
	unsigned char * buff = reinterpret_cast<unsigned char *>(&address);

	return		Converter::numberToStr((unsigned int)buff[0])
		+ "." + Converter::numberToStr((unsigned int)buff[1])
		+ "." + Converter::numberToStr((unsigned int)buff[2])
		+ "." + Converter::numberToStr((unsigned int)buff[3])
	;
}