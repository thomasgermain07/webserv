/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Time.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/02 01:18:54 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/24 18:05:13 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Time.hpp"

Time::t_time
Time::getTime()
{
	timeval now;

	if (gettimeofday(&now, NULL) == -1)
		return 0;

	return now.tv_sec;
}

std::string
Time::getUTCString()
{
	// https://tools.ietf.org/html/rfc7231#section-7.1.1.1

	timeval now;
	char buffer[30];

	if (gettimeofday(&now, NULL) == -1)
		return "";

	size_t length = strftime(buffer, 30, "%a, %d %b %Y %T GMT", gmtime(&now.tv_sec));

	if (length != 29)
		return "";

	return std::string(buffer, 0, length);
}
