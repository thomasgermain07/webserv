/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/06 15:16:35 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/07 21:42:12 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_URI_HPP
# define HTTP_URI_HPP

# include <string>

struct URI
{
private:
	// Static const
	static const std::string SEP_PROTOCOL;
	static const std::string SEP_HOST;
	static const std::string SEP_PORT;
	static const std::string SEP_PATH;
	static const std::string SEP_QUERY;

public:
	// Attributes
	std::string raw;

	std::string protocol;
	std::string host;
	std::string port;
	std::string path;
	std::string query;
	std::string anchor;

public:
	// Constructors & Destructor
	URI(std::string raw);

	// Getters & Setters
	std::string getHost() const;
	std::string getRootPath(const std::string & root) const;

private:
	// C++98 Delete
	URI(const URI & other);
	URI & operator=(const URI & other);
};

#endif
