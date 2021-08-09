/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/03 16:20:31 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 20:37:01 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include "Types.hpp"
#include "utils/Converter.hpp"
#include "utils/Network.hpp"
#include "utils/String.hpp"
#include "logger/Console.hpp"
#include "URI.hpp"
#include "CGI.hpp"

/**
 * Constructors & Destructor
 */
CGI::CGI(
	char *envp[],
	const std::string & resource_path,
	const Server & server,
	const Location & location,
	const Request & request,
	const Connection & connection
)
	: script_path_()
	, resource_path_(resource_path)
	, server_(server)
	, location_(location)
	, request_(request)
	, connection_(connection)
{

	this->setScriptPath();

	if (this->isCallable())
		this->setEnv(envp);
}

/**
 * Methods
 */
void
CGI::call()
{
	// Console::log("CGI::call() : Begin.");

	pid_t pid;
	t_fd pipes[2];

	int exit_code = 0;

	char ** args = this->getArgs();
	char ** env = this->getEnv();

	if (pipe(pipes) < 0)
	{
		throw std::runtime_error("CGI::call() : 'pipe()' failed.");
	}

	if ((pid = fork()) < 0)
	{
		throw std::runtime_error("CGI::call() : 'fork()' failed.");
	}

	// Child
	if (pid == 0)
	{
		// Close 'pipe[in]'
		close(pipes[PIPE_IN]);

		// Create 'temporary' file
		t_fd tmp_fd_out;
		if ((tmp_fd_out = open("/tmp/webserver_cgi_out", O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0)
			exit(1);

		// Redirect 'pipe[out]' into 'stdin' (to 'read' POST data)
		if (dup2(pipes[PIPE_OUT], STD_IN) < 0)
			exit(1);
		// Redirect 'cgi' output into the 'temporary' file
		if (dup2(tmp_fd_out, STD_OUT) < 0)
			exit(1);

		// Close 'pipe[out]'
		close(tmp_fd_out);
		close(pipes[PIPE_OUT]);

		// Execute
		exit_code = execve(args[0], args, env);

		exit(exit_code);
	}
	// Parent
	else
	{
		// Deleting 'execve' args and env
		if (args)
		{
			for (size_t i = 0; args[i]; ++i)
				String::del(args[i]);
			delete[] args;
		}
		if (env)
		{
			for (size_t i = 0; env[i]; ++i)
				String::del(env[i]);
			delete[] env;
		}

		// Pipes
		close(pipes[PIPE_OUT]);
		// Write 'POST' data into 'pipe[in]'
		write(pipes[PIPE_IN], &this->request_.body.c_str()[0], this->request_.body.length());

		close(pipes[PIPE_IN]);

		// Wait 'CGI' script to finish
		// Console::log("CGI::call() : waiting...");

		if (waitpid(pid, &exit_code, 0) < 0)
		{
			throw std::runtime_error("CGI::call() : 'waitpid()' failed.");
		}

		if (WIFEXITED(exit_code))
		{
			if (WEXITSTATUS(exit_code) == 0)
			{
				// Success
				return ;
			}
		}
	}

	throw std::runtime_error("CGI::call() : Something went wrong.");
}

void
CGI::updateResponse(
	Response & response,
	bool set_body
)
{
	// Console::log("CGI::updateResponse() : Begin.");

	// Open temporary file which contains 'cgi' output
	std::ifstream file("/tmp/webserver_cgi_out");

	if (file.fail())
		throw std::runtime_error("CGI::updateResponse() : CGI output file opening failed.");

	// Set Status
	response.status = Response::OK;

	// Read 'cgi' output
	// std::stringstream ss;
	// ss << file.rdbuf();

	// std::string file_content = ss.str();
	// std::string::size_type headers_end = file_content.find("\r\n\r\n");

	// // No headers
	// if (headers_end == std::string::npos)
	// 	return ;

	// file_content.clear();

	// Headers
	std::string line;
	while (std::getline(file, line))
	{
		// Console::log("CGI::updateResponse() : Line : " + line.substr(0, 50));

		// Empty line
		if (line.empty() || (line.length() == 1 && line[0] == '\r'))
			break;

		// Header
		std::string::size_type colon = line.find(":");

		if (colon == std::string::npos)
			throw std::runtime_error("CGI::updateResponse() : Invalid CGI output.");

		std::string h_name = line.substr(0, colon);
		std::string h_value = line.substr(colon + 1);

		String::trim(h_value);

		if (h_name == "Status")
			response.status = static_cast<Response::eStatus>(Converter::strToNumber<int>(h_value));
		else if (h_name == "Content-type")
			response.headers["Content-Type"] = h_value;
		else
			response.headers[h_name] = h_value;
	}

	// Content
	while (std::getline(file, line))
	{
		// Console::log("CGI::updateResponse() : Content : " + line.substr(0, 50));
		response.body.append(line + "\n");
	}
	// Remove last '\n'
	if (!response.body.empty())
		response.body.resize(response.body.length() - 1);

	// Add 'Content-Length' header
	response.headers["Content-Length"] = Converter::numberToStr(response.body.length());

	if (!set_body)
	{
		response.body.clear();
	}

	// Close temporary file
	file.close();
}

/**
 * Getters & Setters
 */
bool
CGI::isCallable() const
{
	return !this->script_path_.empty();
}

/**
 * Private methods
 */
char **
CGI::getArgs() const
{
	char ** args = new char*[3];

	args[0] = String::duplicate(this->script_path_);
	args[1] = String::duplicate(this->resource_path_);
	args[2] = 0;

	return args;
}

char **
CGI::getEnv() const
{
	// Allocation memory
	char **env = new char*[this->env_.size() + 1];

	// Convert
	t_env_ctn::size_type i = 0;

	for (t_env_ctn::const_iterator env_it = this->env_.begin(); env_it != this->env_.end(); ++env_it)
	{
		env[i++] = String::duplicate(env_it->first + "=" + env_it->second);
	}
	env[i] = 0;

	return env;
}

void
CGI::setScriptPath()
{
	// Get 'file' extension
	std::string ext = String::getExtension(this->resource_path_);

	// Check 'cgi' defined
	typedef Location::t_cgi_ctn		t_loc_cgi_ctn;

	for (t_loc_cgi_ctn::const_iterator cgi_it = this->location_.cgi.begin(); cgi_it != this->location_.cgi.end(); ++cgi_it)
	{
		if (cgi_it->first == ext)
		{
			this->script_path_ = cgi_it->second;
			break ;
		}
	}
}

void
CGI::setEnv(
	char * envp[]
)
{
	// Envp
	if (envp)
	{
		size_t i = 0;
		while (envp[i])
		{
			std::string tmp = envp[i];
			std::string::size_type equals = tmp.find("=");

			this->env_[tmp.substr(0, equals)] = tmp.substr(equals + 1);
			++i;
		}
	}

	// Meta variables (from subject)
	URI uri(this->request_.uri);

	if (this->request_.headers.count("Content-Type"))
		this->env_["CONTENT_TYPE"]	= this->request_.headers.find("Content-Type")->second;
	this->env_["AUTH_TYPE"]			= "";
	this->env_["CONTENT_LENGTH"]	= Converter::numberToStr(this->request_.body.length());
	this->env_["DOCUMENT_ROOT"]		= this->resource_path_.substr(0, this->resource_path_.rfind("/"));
	this->env_["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->env_["PATH_INFO"]			= uri.path;
	this->env_["PATH_TRANSLATED"]	= this->resource_path_;
	this->env_["QUERY_STRING"]		= uri.query;
	this->env_["REDIRECT_STATUS"]	= "200";
	this->env_["REMOTE_ADDR"]		= this->connection_.getIP();
	this->env_["REMOTE_IDENT"]		= "";
	this->env_["REMOTE_USER"]		= "";
	this->env_["REQUEST_METHOD"]	= this->request_.method;
	this->env_["REQUEST_URI"]		= this->request_.uri;
	this->env_["SCRIPT_NAME"]		= this->resource_path_;
	this->env_["SERVER_NAME"]		= Network::addrToStr(this->server_.host);
	this->env_["SERVER_PORT"]		= this->server_.port;
	this->env_["SERVER_PROTOCOL"]	= "HTTP/1.1";
	this->env_["SERVER_SOFTWARE"]	= "WebServ/1.0";

	// HTTP Header
	typedef Request::t_headers_ctn		t_req_header_ctn;

	std::string h_name;

	for (t_req_header_ctn::const_iterator h_it = this->request_.headers.begin(); h_it != this->request_.headers.end(); ++h_it)
	{
		h_name = h_it->first;

		String::replaceAll(h_name, "-", "_");
		String::toUpper(h_name);

		this->env_["HTTP_" + h_name] = h_it->second;
	}
}
