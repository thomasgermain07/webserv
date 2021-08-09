/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 17:31:23 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/26 13:41:00 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/**
 * Static
 */
Request::eMethod
Request::strToMethod(
	const std::string & str
)
{
	eMethod method;

	if (str == "GET")			method = Request::GET;
	else if (str == "HEAD")		method = Request::HEAD;
	else if (str == "POST")		method = Request::POST;
	else if (str == "PUT")		method = Request::PUT;
	else if (str == "DELETE")	method = Request::DELETE;
	else if (str == "CONNECT")	method = Request::CONNECT;
	else if (str == "OPTIONS")	method = Request::OPTIONS;
	else if (str == "TRACE")	method = Request::TRACE;
	else						method = Request::UNKNOWN;

	return method;
}

std::string
Request::methodToStr(
	eMethod method
)
{
	std::string str;

	switch (method)
	{
		case GET:		str = "GET";		break;
		case HEAD:		str = "HEAD";		break;
		case POST:		str = "POST";		break;
		case PUT:		str = "PUT";		break;
		case DELETE:	str = "DELETE";		break;
		case CONNECT:	str = "CONNECT";	break;
		case OPTIONS:	str = "OPTIONS";	break;
		case TRACE:		str = "TRACE";		break;
		default:		str = "";			break;
	}

	return str;
}

std::string
Request::getKnownMethods()
{
	return "GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE";
}