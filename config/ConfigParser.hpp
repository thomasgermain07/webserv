/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/26 15:36:27 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/24 18:10:00 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include <list>
# include <map>
# include <fstream>
# include "Config.hpp"
# include "Location.hpp"
# include "Server.hpp"
# include "Types.hpp"

class ConfigParser
{
public :	// Config Enum
	enum eConfigField
	{
		LISTEN = 0,
		SERVER,
		NAME,
		ROOT,
		ERROR,
		BODY_SIZE,
		LOCATION,
		METHOD,
		AUTO_INDEX,
		INDEXES,
		UPLD_DIR,
		AUTH_NAME,
		AUTH_FILE,
		CGI_EXT,
		CLOSE_BRACKET,
		UNKNOWN
	};

public : // Typedef
	typedef Config::t_cgi_ctn				t_cgi_ctn;
	typedef Config::t_loc_ctn				t_loc_ctn;
	typedef Config::t_err_ctn				t_err_ctn;
	typedef Config::t_str_ctn				t_str_ctn;
	typedef Config::t_method_ctn			t_method_ctn;

private :	// Private Attributes
	Config				cfg;
	const std::string	file_path_;
	std::ifstream		file;

private :	// Parsing Status
	bool		in_server;
	bool		in_location;
	bool		redefinition;
	Server		*srv;
	Location	*loc;
	size_t		line;
	std::string	srv_name;

public :	// Constructor /  Destructor
	ConfigParser(const std::string & file_path, char *envp[]);
	~ConfigParser();

public :	// Getter
	const Config &	getConfig() const;

private : // Private Methods
	void		openFile();
	void		parseFile();
	void		initLocation(std::string & buffer);

private : // Parsing fonction
	void		parseString(const std::string & buffer, std::string & destination);
	void		parseBool(const std::string & buffer, bool & destination);
	void		parseHost(std::string & buffer, in_addr_t & host, t_port & port);
	void		parseContString(std::string & buffer, t_str_ctn & destination);
	void		parseMethod(std::string & buffer, t_method_ctn & destination);
	void		parseError(std::string & buffer, t_err_ctn & destination);
	void		parseMaxBodySize(std::string & buffer, size_t & destination);
	void		parseCgiExt(std::string & buffer, t_cgi_ctn	& destination);

private : // Checking function
	void		checkLocation();
	void		checkServer();

private : // Utils Methods
	void		trim(std::string & buffer) const;
	void		displayError(std::string message) const;
	bool		isKnownStatus(const Response::eStatus & status) const;

private : // Private Getter
	eConfigField	getField(std::string & buffer) const;
};

#endif
