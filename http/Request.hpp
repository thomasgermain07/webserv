/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/27 15:06:15 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/07 23:39:37 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <string>
# include <map>

struct Request
{
	// HTTP Methods
	enum eMethod
	{
		// Supported
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE,
		// Error
		UNKNOWN
	};

	// Static
	static eMethod strToMethod(const std::string & str);
	static std::string methodToStr(eMethod method);
	static std::string getKnownMethods();

	// Typedef
	typedef std::map<std::string, std::string>	t_headers_ctn;

	// Attributes
	std::string			method;
	std::string			uri;
	int					http_version_major;
	int					http_version_minor;
	t_headers_ctn		headers;
	std::string			body;
};

#endif
