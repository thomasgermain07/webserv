/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/26 15:36:27 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/18 19:13:44 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_CONFIG_HPP
# define CONFIG_CONFIG_HPP

# include <list>
# include <map>
# include <vector>
# include "Types.hpp"
# include "Response.hpp"
# include "Request.hpp"

struct Location;
struct Server;

struct Config
{
	// Typedef
	typedef std::map<std::string, Location *>			t_loc_ctn;
	typedef std::map<Response::eStatus, std::string>	t_err_ctn;
	typedef std::vector<std::string>					t_str_ctn;
	typedef std::map<std::string, std::string>			t_cgi_ctn;
	typedef std::vector<Request::eMethod>				t_method_ctn;

	enum eConfigField
	{
		LISTEN = 0,
		NAME,
		ROOT,
		ERROR,
		BODY_SIZE,
		LOCATION,
		URI,
		METHOD,
		AUTO_INDEX,
		INDEXES,
		UPLD_DIR,
		ALLOW_UPLD,
		PSWRD_AUTH,
		PSWRD_FILE,
		CGI_EXT,
		CLOSE_BRACKET,
		UNKNOWN
	};

	// Typedef
	typedef std::list<Server *>					t_server_ctn;
	typedef std::map<t_port, t_server_ctn>		t_srv_map_ctn;

	// Attributes
	t_srv_map_ctn	srv_map;	// port : list of 'server' on it
	char **			envp;

	// Destructor
	~Config();
};

#endif
