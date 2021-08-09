/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/22 17:58:37 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/24 23:07:18 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_RESOURCE_HPP
# define UTILS_RESOURCE_HPP

# include <string>
# include <vector>
# include <map>
# include "config/Config.hpp"
# include "Response.hpp"

class Resource
{
public:
	// Typedef
	typedef std::vector<std::string>			t_folder_content_ctn;

	// Static
	static size_t get(const std::string & path, std::string * body);
	static size_t getListing(const std::string & path, const Request & request, std::string * body);

	static size_t getError(const Config::t_err_ctn & err_map, const std::string & root, std::string * body, Response::eStatus err);
	static size_t generateErrorPage(Response::eStatus code, std::string * body);

	static void save(const std::string & path, const std::string & content);
	static void del(const std::string & path);

	static bool fullyExists(const std::string & path, const std::string & body);

private:
	// C++98 Delete
	Resource();
	Resource(const Resource & other);
	Resource & operator=(const Resource & other);
};

#endif