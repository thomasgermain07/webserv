/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/03 16:19:07 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/18 19:16:37 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CGI_HPP
# define HTTP_CGI_HPP

# include <map>
# include <string>
# include "config/Server.hpp"
# include "config/Location.hpp"
# include "config/Config.hpp"
# include "Connection.hpp"
# include "Request.hpp"

class CGI
{
private:
	// Typedef
	typedef std::map<std::string, std::string>		t_env_ctn;

private:
	// Attributes
	t_env_ctn			env_;

	std::string			script_path_;
	const std::string &	resource_path_;
	const Server &		server_;
	const Location &	location_;
	const Request & 	request_;
	const Connection &	connection_;

public:
	// Constructors & Destructor
	CGI(char * envp[], const std::string & resource_path, const Server & server, const Location & location, const Request & request, const Connection & connection);

	// Methods
	void call();
	void updateResponse(Response & response, bool set_body = false);

	// Getters & Setters
	bool isCallable() const;

private:
	// Private methods
	char ** getArgs() const;
	char ** getEnv() const;

	void setScriptPath();
	void setEnv(char *envp[]);

};

#endif
