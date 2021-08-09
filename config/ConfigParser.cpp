/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/14 15:02:15 by thgermai          #+#    #+#             */
/*   Updated: 2021/03/01 18:56:42 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ConfigParser.hpp"
#include "Converter.hpp"
#include "utils/System.hpp"
#include "utils/String.hpp"

ConfigParser::ConfigParser(const std::string & file_path, char *envp[])
	: file_path_(file_path),
	  in_server(false),
	  in_location(false),
	  redefinition(false),
	  srv(NULL),
	  loc(NULL),
	  line(0)
{
	openFile();
	parseFile();
	file.close();
	cfg.envp = envp;
}

ConfigParser::~ConfigParser()
{

}

void
ConfigParser::parseFile()
{
	std::string		buffer;

	while (getline(file, buffer))
	{
		line++;

		trim(buffer);

		if (buffer.empty())
			continue ;

		eConfigField	field = getField(buffer);

		// Check for unknown field
		if (field == UNKNOWN)
			displayError("Unknwon parameter");

		// Check for server or location creation
		if (field == SERVER)
		{
			if (in_server == true)
				displayError("Server definition inside a server");

			// Create Server and switch status to server
			srv = new Server();
			in_server = true;

			continue ;
		}
		else if (field == LOCATION)
		{
			if (in_server == false)
				displayError("Location definition out of server");

			// Check server params before going to locations definition;
			if (srv->locations.empty())
				checkServer();

			if (in_location == true)
				displayError("Location definition inside a location");

			// Create Location and switch status to location
			loc = new Location();
			in_location = true;
			redefinition = false;
			initLocation(buffer);

			continue ;
		}

		// Check for outside server or location parameter
		if (in_server == false && in_location == false)
			displayError("Parameter given outside server or location");

		// Check for close brackets and add location or server
		if (field == CLOSE_BRACKET)
		{
			if (in_location == true)
			{
				// Check Location params
				checkLocation();

				// Add location to server
				srv->locations[loc->uri] = loc;

				// Clean loc and switch location status off
				loc = NULL;
				in_location = false;
				redefinition = true;
			}
			else if (in_server == true)
			{
				// Check params only if locations map is empty
				if (srv->locations.empty())
					checkServer();

				// Add server to config
				cfg.srv_map[srv->port].push_back(srv);

				// Clean srv and switch servert status off
				srv = NULL;
				in_server = false;
				redefinition = false;
			}
			else
				displayError("Lost close bracket");

			continue ;
		}

		// If server params are given after location -> error
		if (redefinition == true)
			displayError("Server parameter definition after location(s)");

		// Check for forbidden param in server
		if (in_server == true && in_location == false)
		{
			if (field == METHOD)
				displayError("Unvalid parameter in server");
		}

		// Check for forbidden param in location
		if (in_location == true)
		{
			if (field == LISTEN || field == NAME)
				displayError("Unvalid parameter in location");
		}

		// Erase ';' in arguments
		buffer.erase(buffer.size() - 1);

		// Set string destination to send to parseString
		std::string * str_destination = NULL;
		if (field == ROOT || field == UPLD_DIR)
		{
			if (in_location)
				str_destination = (field == ROOT) ? &loc->root : &loc->upload_dir;
			else
				str_destination = (field == ROOT) ? &srv->root : &srv->upload_dir;
		}
		else if (field == AUTH_NAME || field == AUTH_FILE)
		{
			if (in_location)
			{
				if (field == AUTH_NAME && buffer == "off")
					continue ;
				str_destination = (field == AUTH_NAME) ? &loc->authenticate.auth_name : &loc->authenticate.file_path;
				loc->authenticate.active = true;
			}
			else
			{
				if (field == AUTH_NAME && buffer == "off")
					continue ;
				str_destination = (field == AUTH_NAME) ? &srv->authenticate.auth_name : &srv->authenticate.file_path;
				srv->authenticate.active = true;
			}
		}

		// Set t_str_ctn to send to parseConstSring
		t_str_ctn * str_ctn_destination = NULL;
		if (field == INDEXES)
			str_ctn_destination = (in_location == true) ? &loc->indexes : &srv->indexes;

		// Redirect to parsing function
		switch (field)
		{
			// Exclusif Server
			case LISTEN :
				parseHost(buffer, srv->host, srv->port);
				break ;
			case NAME :
				if (srv_name.size())
					srv->names.push_back(srv_name);
				parseContString(buffer, srv->names);
				break ;
			// Exclusif Location
			case METHOD :
				parseMethod(buffer, loc->methods);
				break ;
			// Commun
			case UPLD_DIR : case ROOT : case AUTH_FILE : case AUTH_NAME :
				if (str_destination->empty() == false)
					displayError("Redefinition of parameter");
				parseString(buffer, *str_destination);
				break ;
			case AUTO_INDEX :
				if (in_location)
					parseBool(buffer, loc->auto_index);
				else
					parseBool(buffer, srv->auto_index);
				break ;
			case INDEXES :
				if (in_location)
					parseContString(buffer, loc->indexes);
				else
					parseContString(buffer, srv->indexes);
				break ;
			case ERROR :
				if (in_location)
					parseError(buffer, loc->error_pages);
				else
					parseError(buffer, srv->error_pages);
				break ;
			case BODY_SIZE :
				if (in_location)
					parseMaxBodySize(buffer, loc->client_max_body_size);
				else
					parseMaxBodySize(buffer, srv->client_max_body_size);
				break ;
			case CGI_EXT :
				if (in_location)
					parseCgiExt(buffer, loc->cgi);
				else
					parseCgiExt(buffer, srv->cgi);
				break ;
			default :
				break ;
		}
	}
}

void
ConfigParser::checkLocation()
{
	// Set root with server root + uri if no redefinition of root in location
	if (loc->root.empty())
	{
		loc->root = srv->root + "/" + loc->uri;

		String::replaceAll(loc->root, "//", "/");
	}
	// Verify root path
	else if (!System::isFolder(loc->root))
	{
		displayError("Root is unvalid in this location");
	}

	// Verify upload_dir path
	if (loc->upload_dir.size() && System::isFolder(loc->upload_dir) == false)
		displayError("Upload_dir is unvalid in this location");

	if (loc->authenticate.active == true)
	{
		if (loc->authenticate.auth_name.empty())
			displayError("Missing auth_name in this location");
		if (loc->authenticate.file_path.empty())
			displayError("Missing file_path in this location");
		// Verify file_path path
		else if (System::isFile(loc->authenticate.file_path) == false)
			displayError("Path to authenticate file is unvlaid in this location");
	}

	loc->error_pages.insert(srv->error_pages.begin(), srv->error_pages.end());
	loc->cgi.insert(srv->cgi.begin(), srv->cgi.end());
}

void
ConfigParser::checkServer()
{
	// If Listen isn't set
	if (srv_name.empty())
		displayError("Missing 'Listen' in server");

	// If Root isn't set
	if (srv->root.empty())
		displayError("Missing 'Root' in server");
	// Verify root path
	else if (System::isFolder(srv->root) == false)
		displayError("Root is unvalid in this server");

	// Verify upload_dir path
	if (srv->upload_dir.size() && System::isFolder(srv->upload_dir) == false)
		displayError("Upload_dir is unvalid in this server");

	if (srv->authenticate.active == true)
	{
		if (srv->authenticate.auth_name.empty())
			displayError("Missing auth_name in this server");
		if (srv->authenticate.file_path.empty())
			displayError("Missing file_path in this server");
		// Verify file_path path
		else if (System::isFile(srv->authenticate.file_path) == false)
			displayError("Path to authenticate file is unvlaid in this server");
	}

	if (srv->names.empty())
		srv->names.push_back(srv_name);

	for (Config::t_err_ctn::iterator it = srv->error_pages.begin(); it != srv->error_pages.end(); ++it)
	{
		if (!it->second.empty() && it->second[0] == '/')
			continue ;

		it->second = srv->root + "/" + it->second;

		String::replaceAll(it->second, "//", "/");
	}

	for (Config::t_cgi_ctn::iterator it = srv->cgi.begin(); it != srv->cgi.end(); ++it)
	{
		if (!it->second.empty() && it->second[0] == '/')
			continue ;

		it->second = srv->root + "/" + it->second;

		String::replaceAll(it->second, "//", "/");
	}
}

void
ConfigParser::parseString(const std::string & buffer, std::string & destination)
{
	destination = buffer;
}

void
ConfigParser::parseBool(const std::string & buffer, bool & destination)
{
	if (buffer == "on")
		destination = true;
	else if (buffer == "off")
		destination = false;
	else
		displayError("Unvalid statement (on/off)");
}

void
ConfigParser::parseHost(std::string & buffer, in_addr_t & host, t_port & port)
{
	size_t		pos = buffer.find(':');
	std::string host_str;

	if (port != 0)
		displayError("Redefinition of host in server");

	// Check if host is specifier or listen to all address
	if (pos == std::string::npos || pos == 0)
		host_str = "0.0.0.0";
	else
		host_str = buffer.substr(0, pos);

	srv_name = host_str;

	// Check if listen is declared after Names
	if (srv->names.empty() == false)
		srv->names.push_back(srv_name);

	// Delete address + ':'
	if (pos != std::string::npos)
		buffer.erase(0, pos + 1);

	// Parse host and convert it to host
	host = inet_addr(host_str.c_str());
	port = Converter::strToNumber<size_t>(buffer);
}

void
ConfigParser::parseContString(std::string & buffer, t_str_ctn & destination)
{
	size_t		pos;

	while ((pos = buffer.find_first_of(" \t")) != std::string::npos)
	{
		std::string token = buffer.substr(0, pos);
		destination.push_back(token);
		pos = buffer.find_first_not_of(" \t", pos);
		buffer.erase(0, pos);
	}
	if (buffer.size())
		destination.push_back(buffer);
}

void
ConfigParser::parseMethod(std::string & buffer, t_method_ctn & destination)
{
	size_t		pos;

	while ((pos = buffer.find_first_of(" \t")) != std::string::npos)
	{
		std::string method_str = buffer.substr(0, pos);

		// Convert str to eMethod
		Request::eMethod method = Request::strToMethod(method_str);
		if (method == Request::UNKNOWN)
			displayError("Unknown method");

		destination.push_back(method);
		pos = buffer.find_first_not_of(" \t", pos);
		buffer.erase(0, pos);
	}
	if (buffer.size())
	{
		Request::eMethod method = Request::strToMethod(buffer);
		if (method == Request::UNKNOWN)
			displayError("Unknown method");

		destination.push_back(method);
	}
}

void
ConfigParser::parseError(std::string & buffer, t_err_ctn & destination)
{
	size_t	pos = buffer.find_last_of(" \t");
	// Check if only 1 code is given without path
	if (pos == std::string::npos)
		displayError("No path specified");

	// Get last arg and check if it's a path
	std::string path = buffer.substr(pos + 1);

	// Delete path to keep only codes
	buffer.erase(pos);

	Response::eStatus code;

	while ((pos = buffer.find_first_of(" \t")) != std::string::npos)
	{
		std::string code_str = buffer.substr(0, pos);
		code = Response::eStatus(Converter::strToNumber<int>(code_str));

		// Check is code is valid
		if (isKnownStatus(code) == false)
			displayError("Unknown code");

		if (destination.find(code) != destination.end())
			displayError("Double redefinition of code " + code_str);
		else
			destination[code] = path;

		pos = buffer.find_first_not_of(" \t", pos);
		buffer.erase(0, pos);
	}
	code = Response::eStatus(Converter::strToNumber<int>(buffer));
	// Check is code is valid
	if (isKnownStatus(code) == false || code == 0)
		displayError("Unknown code");

	if (destination.find(code) != destination.end())
		displayError("Double redefinition of code " + buffer);
	else
		destination[code] = path;
}

void
ConfigParser::parseMaxBodySize(std::string & buffer, size_t & destination)
{
	size_t		mult = 1;

	if (buffer[buffer.size() - 1] == 'm' || buffer[buffer.size() - 1] == 'M')
		mult = 1000000;
	else if (buffer[buffer.size() - 1] == 'k' || buffer[buffer.size() - 1] == 'K')
		mult = 1000;

	// Erase m or k
	if (mult != 1)
		buffer.erase(--buffer.end());

	size_t res = Converter::strToNumber<size_t>(buffer);

	destination = res * mult;
}

void
ConfigParser::parseCgiExt(std::string & buffer, t_cgi_ctn & destination)
{
	size_t		pos = buffer.find_first_of(" \t");

	if (pos == std::string::npos)
		displayError("Missing executable for extension");

	std::string extension = buffer.substr(0, pos);

	// Add '.' if it isn't specified
	if (extension[0] != '.')
		extension = "." + extension;

	pos = buffer.find_first_not_of(" \t", pos);
	buffer.erase(0, pos);

	destination[extension] = buffer;
}

void
ConfigParser::initLocation(std::string & buffer)
{
	// Set location URI
	size_t		pos = buffer.find_first_of(" {");

	// Check if missing URI
	if (pos == 0)
		displayError("Missing URI for this location");

	loc->uri = buffer.substr(0, pos);
}

void
ConfigParser::openFile()
{
	file.open(file_path_.c_str());

	if (file.fail() == true)
		throw std::runtime_error("Config::openFile() : failed to open");
}

const Config &
ConfigParser::getConfig() const
{
	return cfg;
}

void
ConfigParser::trim(std::string & buffer) const
{
	if (buffer.empty())
		return ;

	size_t		start = buffer.find_first_not_of(" \t");
	size_t		end = buffer.find_last_of(";{}");

	if (end == std::string::npos)
		displayError("Missing ';'");

	buffer = buffer.substr(start, end + 1);
}

ConfigParser::eConfigField
ConfigParser::getField(std::string & buffer) const
{
	// Get field
	size_t		pos = buffer.find_first_of(" \t");
	std::string field = buffer.substr(0, pos);

	if (field[0] == '}')
		return CLOSE_BRACKET;

	// Delete field and whitespace before agruments
	pos = buffer.find_first_not_of(" \t", pos);
	buffer.erase(0, pos);

	// Find ConfigField with size
	size_t field_size = field.size();
	switch (field_size)
	{
	case 4 :
		return	field == "root" ? ROOT :
				UNKNOWN;
	case 5 :
		return	field == "index" ? INDEXES :
				UNKNOWN;
	case 6 :
		return	field == "listen" ? LISTEN :
				field == "server" ? SERVER :
				UNKNOWN;
	case 7 :
		return	field == "cgi_ext" ? CGI_EXT :
				UNKNOWN;
	case 8 :
		return	field == "location" ? LOCATION :
				UNKNOWN;
	case 9 :
		return	field == "autoindex" ? AUTO_INDEX :
				UNKNOWN;
	case 10 :
		return	field == "error_page" ? ERROR :
				field == "upload_dir" ? UPLD_DIR :
				field == "auth_basic" ? AUTH_NAME :
				UNKNOWN;
	case 11 :
		return	field == "server_name" ? NAME :
				UNKNOWN;
	case 13 :
		return	field == "allow_request" ? METHOD :
				UNKNOWN;
	case 20 :
		return	field == "auth_basic_user_file" ? AUTH_FILE :
				field == "client_max_body_size" ? BODY_SIZE :
				UNKNOWN;
	default :
		return UNKNOWN;
		break;
	}
}

bool
ConfigParser::isKnownStatus(const Response::eStatus & status) const
{
	std::string	buffer = Response::statusToStr(status);

	if (buffer == "Undefined")
		return false;
	return true;
}

void
ConfigParser::displayError(std::string message) const
{
	if (in_location)
		delete loc;
	if (in_server)
		delete srv;
	throw std::runtime_error(file_path_ + " : line " + Converter::numberToStr(line) + " : " + message);
}