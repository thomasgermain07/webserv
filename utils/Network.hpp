/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Network.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/07 18:59:30 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/07 23:15:10 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_NETWORK_HPP
# define UTILS_NETWORK_HPP

# include <netinet/in.h>
# include <string>

class Network
{
public:
	// Static
	static std::string addrToStr(in_addr_t address);

};

#endif
