/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/27 21:20:04 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 18:59:42 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_SERVER_HPP
# define CONFIG_SERVER_HPP

# include <arpa/inet.h>
# include <vector>
# include "Config.hpp"
# include "Authenticate.hpp"
# include "Types.hpp"
# include "Response.hpp"

// If include "Location" circular dependency
struct Location;

struct Server
{
	// Typedef
	typedef Config::t_loc_ctn	t_loc_ctn;
	typedef Config::t_err_ctn	t_err_ctn;
	typedef Config::t_str_ctn	t_str_ctn;
	typedef Config::t_cgi_ctn	t_cgi_ctn;

	// Exclusif Attributes
	in_addr_t 				host;
	t_port					port;
	t_str_ctn				names;

	// Commun Attributes
	std::string				root;
	size_t					client_max_body_size;
	Authenticate			authenticate;
	t_err_ctn				error_pages;
	t_str_ctn				indexes;
	bool					auto_index;
	std::string				upload_dir;
	t_cgi_ctn				cgi; // extension : executable

	// Locations
	t_loc_ctn				locations;	// path : path cfg

	// Constructor
	Server();
	// Destructor
	~Server();
};

#endif
