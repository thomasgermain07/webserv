/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestInterpreter.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/16 16:00:29 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 22:54:13 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger/Console.hpp"
#include "utils/Time.hpp"
#include "utils/Converter.hpp"
#include "utils/Network.hpp"
#include "utils/String.hpp"
#include "utils/System.hpp"
#include "Authenticator.hpp"
#include "Mime.hpp"
#include "CGI.hpp"
#include "URI.hpp"
#include "RequestInterpreter.hpp"

/**
 * Constructors & Destructor
 */
RequestInterpreter::RequestInterpreter(
	const Config & cfg,
	t_port port
)
	: cfg_(cfg)
	, port_(port)
	, srv_()
	, loc_()
{
	// Empty
}

RequestInterpreter::~RequestInterpreter()
{
	// Empty
}

/**
 * Methods
 */
void RequestInterpreter::exec(
	const Request & request,
	const Connection * connection
)
{
	// Set 'loc' to an empty location to handle error without crashing
	Location new_loc;
	this->loc_ = &new_loc;

	// Set 'connection'
	this->conn_ = connection;

	// Check 'Host' is defined (mandatory HTTP/1.1)
	if (!request.headers.count("Host") || request.headers.find("Host")->second.empty())
	{
		Console::log("RequestInterpreter::exec() : 'Host' is not defined (or empty).");
		this->execError(Response::BAD_REQUEST, false);
		return ;
	}

	// Set 'server' and 'location' matching the 'request'
	this->srv_ = 0;
	this->setServer(request);

	if (!this->srv_)
	{
		this->execError(Response::INTERNAL_SERVER_ERROR, false);
		return ;
	}

	// Reset this->loc_
	this->loc_ = NULL;
	this->setLocation(request);

	// If no known location, create new location from server param
	if (!this->loc_)
		initLocationFromServer(new_loc);

	// Check 'request' errors
	URI uri(request.uri);
	if (!uri.host.empty()
		&& uri.host != request.headers.find("Host")->second
		&& uri.getHost() != request.headers.find("Host")->second
	)
	{
		Console::log("RequestInterpreter::exec() : 'Host' in request 'uri' does not match 'Host' header.");
		this->execError(Response::BAD_REQUEST);
		return ;
	}
	if (this->loc_->client_max_body_size && request.body.length() > this->loc_->client_max_body_size)
	{
		this->execError(Response::PAYLOAD_TOO_LARGE);
		return ;
	}

	// Check 'request' method
	typedef Location::t_method_ctn		t_loc_method_ctn;

	if (Request::strToMethod(request.method) == Request::UNKNOWN)
	{
		this->execError(Response::NOT_IMPLEMENTED);
		return ;
	}

	bool allowed = this->loc_->methods.empty();

	if (!allowed)
	{
		for (t_loc_method_ctn::const_iterator it_method = this->loc_->methods.begin(); it_method != this->loc_->methods.end(); ++it_method)
		{
			if (*it_method == Request::strToMethod(request.method))
				allowed = true;
		}

		if (!allowed)
		{
			this->execError(Response::METHOD_NOT_ALLOWED);
			return ;
		}
	}

	// Check 'authentication'
	if (this->loc_->authenticate.active)
	{
		try
		{
			if (!request.headers.count("Authorization"))
			{
				throw std::runtime_error("RequestInterpreter::exec() : 'Authorization' is not defined.");
			}

			std::string auth = request.headers.find("Authorization")->second;

			// Check 'type'
			if (auth.rfind("Basic ", 0) != 0)
			{
				throw std::runtime_error("RequestInterpreter::exec() : Invalid authentication type.");
			}
			auth.erase(0, 6);

			Authenticator::authenticate(this->loc_->authenticate.file_path, auth);
		}
		catch (const std::exception & e)
		{
			Console::log(e.what(), Console::ERROR);
			this->execError(Response::UNAUTHORIZED);
			return ;
		}
	}

	// Execute 'request' method
	if      (request.method == "GET")		this->execGet(request);
	else if (request.method == "HEAD")		this->execHead(request);
	else if (request.method == "POST")		this->execPost(request);
	else if (request.method == "PUT")		this->execPut(request);
	else if (request.method == "DELETE")	this->execDelete(request);
	else if (request.method == "CONNECT")	this->execConnect(request);
	else if (request.method == "OPTIONS")	this->execOptions(request);
	else /*(request.method == "TRACE")*/	this->execTrace(request);
}

void RequestInterpreter::execError(
	Response::eStatus status,
	bool load_content // true by default
)
{
	Response * res = this->getDefaultResponse();

	res->status = status;

	if (load_content == true)
	{
		// Get error content and Content-Length
		size_t ret = Resource::getError(loc_->error_pages, loc_->root, &res->body, status);
		res->headers["Content-Length"] = Converter::numberToStr(ret);
	}

	switch (status)
	{
		// https://tools.ietf.org/html/rfc7231#section-6.5.5
		case Response::METHOD_NOT_ALLOWED:
			typedef Location::t_method_ctn		t_loc_method_ctn;

			for (t_loc_method_ctn::const_iterator it_method = this->loc_->methods.begin(); it_method != this->loc_->methods.end(); ++it_method)
			{
				if (it_method != this->loc_->methods.begin())
				{
					res->headers["Allow"] += ", ";
				}

				res->headers["Allow"] += Request::methodToStr(*it_method);
			}

			if (res->headers["Allow"].empty())
			{
				res->headers["Allow"] = Request::getKnownMethods();
			}
			break ;

		// https://tools.ietf.org/html/rfc7231#section-6.5.7
		case Response::REQUEST_TIMEOUT:
			res->headers["Connection"] = "close";
			break ;

		// https://tools.ietf.org/html/rfc7231#section-6.5.1
		case Response::BAD_REQUEST:
			res->headers["Connection"] = "close";
			break ;

		// https://tools.ietf.org/html/rfc7235#section-3.1
		case Response::UNAUTHORIZED:
			res->headers["WWW-Authenticate"] = "Basic realm=\"" + this->loc_->authenticate.auth_name + "\"";
			break ;

		// https://tools.ietf.org/html/rfc7231#section-6.5.4
		case Response::NOT_FOUND:
			break ;

		// https://tools.ietf.org/html/rfc7231#section-6.5.11
		case Response::PAYLOAD_TOO_LARGE:
			res->headers["Connection"] = "close";
			break ;

		// https://tools.ietf.org/html/rfc6585#section-4
		case Response::TOO_MANY_REQUEST:
			res->headers["Retry-After"] = "60";
			break ;

		// https://tools.ietf.org/html/rfc7231#section-6.6.4
		case Response::SERVICE_UNAVAILABLE:
			res->headers["Retry-After"] = "3600";
			res->headers["Connection"] = "close";
			break ;

		// https://tools.ietf.org/html/rfc7231#section-6.6.1
		case Response::INTERNAL_SERVER_ERROR:
			// res->headers["Retry-After"] = "60";
			break ;

		default:
			break ;
	}

	// Append 'response' to the 'queue'
	this->responses_.push(res);
}

/**
 * Getters & Setters
 */
bool RequestInterpreter::hasResponse() const
{
	return !this->responses_.empty();
}

Response *
RequestInterpreter::getResponse()
{
	if (this->responses_.empty())
		return 0; // nullptr

	Response *res = this->responses_.front();
	this->responses_.pop();

	return res;
}

/**
 * Private methods
 */
Response *
RequestInterpreter::getDefaultResponse() const
{
	Response * res = new Response();

	res->status = Response::INTERNAL_SERVER_ERROR;
	res->http_version_major = 1;
	res->http_version_minor = 1;
	res->headers["Server"] = "WebServ";
	res->headers["Date"] = Time::getUTCString();
	res->headers["Content-Length"] = "0";

	return res;
}

void
RequestInterpreter::initLocationFromServer(Location & new_loc)
{
	new_loc.root = this->srv_->root;
	new_loc.client_max_body_size = this->srv_->client_max_body_size;
	new_loc.error_pages = this->srv_->error_pages;
	new_loc.indexes = this->srv_->indexes;
	new_loc.auto_index = this->srv_->auto_index;
	new_loc.upload_dir = this->srv_->upload_dir;
	new_loc.cgi = this->srv_->cgi;
	new_loc.authenticate.active = false;

	this->loc_ = &new_loc;
}

void
RequestInterpreter::execGet(
	const Request & request,
	bool load_content
)
{
	// https://tools.ietf.org/html/rfc7231#section-4.3.1

	Response * res = this->getDefaultResponse();

	std::string resource_path = this->getResourcePath(request);

	// Console::log("RequestInterpreter::execGet() : Before : " + resource_path);

	// Check 'resource' exists
	bool is_file = System::isFile(resource_path);
	bool is_folder = System::isFolder(resource_path);

	// Check 'indexes'
	if (is_folder && !this->loc_->indexes.empty())
	{
		typedef Location::t_str_ctn		t_loc_index_ctn;

		for (t_loc_index_ctn::const_iterator idx_it = this->loc_->indexes.begin(); idx_it != this->loc_->indexes.end(); ++idx_it)
		{
			std::string tmp_resource_path = resource_path + '/' + *idx_it;

			// Erase possible '//' due to previous concatenation
			String::replaceAll(tmp_resource_path, "//", "/");

			// Console::log("RequestInterpreter::execGet() : Trying : " + tmp_resource_path);

			// Check 'resource' exists
			if ((is_file = System::isFile(tmp_resource_path)))
			{
				resource_path = tmp_resource_path;
				break ;
			}
		}
	}

	// Console::log("RequestInterpreter::execGet() : After : " + resource_path);

	std::string::size_type content_length = 0;

	try
	{
		std::string * body_ptr = (load_content) ? &res->body : 0; // nullptr

		// Load 'resource'
		if (is_file)
		{
			CGI cgi(this->cfg_.envp, resource_path, *this->srv_, *this->loc_, request, *this->conn_);

			if (cgi.isCallable())
			{
				try
				{
					cgi.call();
					cgi.updateResponse(*res, load_content);
				}
				catch (const std::exception & e)
				{
					delete res;
					Console::log(e.what());
					this->execError(Response::INTERNAL_SERVER_ERROR, load_content);
					return ;
				}
			}
			else
			{
				content_length = Resource::get(resource_path, body_ptr);

				std::string ext;
				std::string::size_type pos;
				if ((pos = resource_path.rfind("/")) != std::string::npos)
				{
					ext = resource_path.substr(pos);
					if ((pos = ext.rfind(".")) != std::string::npos)
						ext = ext.substr(pos);
				}

				res->headers["Content-Type"] = Mime::extToStr(ext);
			}
		}
		// Load 'auto_index'
		else if (is_folder && this->loc_->auto_index)
		{
			content_length = Resource::getListing(resource_path, request, body_ptr);
			res->headers["Content-Type"] = Mime::extToStr(".html");
		}
	}
	catch (const std::exception & e)
	{
		Console::log(e.what(), Console::ERROR);
		delete res;
		this->execError(Response::INTERNAL_SERVER_ERROR, load_content);
		return ;
	}

	// no file / no index / no listing
	if (res->headers["Content-Type"].empty())
	{
		Console::log("RequestInterpreter::execGet() : No content found.", Console::WARNING);
		delete res;
		this->execError(Response::NOT_FOUND, load_content);
		return ;
	}

	// Update 'response'
	res->status = Response::OK;
	if (res->headers["Content-Length"].empty() || res->headers["Content-Length"] == "0")
		res->headers["Content-Length"] = Converter::numberToStr(content_length);

	// Append 'response' to the 'queue'
	this->responses_.push(res);
}

void
RequestInterpreter::execHead(
	const Request & request
)
{
	// https://tools.ietf.org/html/rfc7231#section-4.3.2

	this->execGet(request, false);
}

void
RequestInterpreter::execPost(
	const Request & request
)
{
	// https://tools.ietf.org/html/rfc7231#section-4.3.3

	Response * res = this->getDefaultResponse();

	std::string resource_path = this->getResourcePath(request);

	CGI cgi(this->cfg_.envp, resource_path, *this->srv_, *this->loc_, request, *this->conn_);

	if (cgi.isCallable())
	{
		try
		{
			cgi.call();
			cgi.updateResponse(*res, true);
		}
		catch (const std::exception & e)
		{
			delete res;
			Console::log(e.what());
			this->execError(Response::INTERNAL_SERVER_ERROR);
			return ;
		}
	}
	else
	{
		delete res;
		this->execPut(request);
		return ;
	}

	// Append 'response' to the 'queue'
	this->responses_.push(res);
}

void
RequestInterpreter::execPut(
	const Request & request
)
{
	// https://tools.ietf.org/html/rfc7231#section-4.3.4

	Response * res = this->getDefaultResponse();

	// Check 'resource'
	std::string resource_path = RequestInterpreter::getResourcePath(request);

	// Invalid 'resource_path'
	if (resource_path.find("..") != std::string::npos)
	{
		Console::log("RequestInterpreter::execPut() : Invalid resource path.", Console::ERROR);
		delete res;
		this->execError(Response::BAD_REQUEST);
		return ;
	}

	// Save 'resource'
	bool exists = System::isFile(resource_path);

	try
	{
		Resource::save(resource_path, request.body);
	}
	catch (const std::exception & e)
	{
		Console::log(e.what(), Console::ERROR);
		delete res;
		this->execError(Response::INTERNAL_SERVER_ERROR);
		return ;
	}

	// Update 'response'
	// 201 (Created)	: if 'resource' did not exists.
	// 204 (No Content)	: if 'resource' did exists and no 'body' is send.
	// 200 (Ok)			: if 'resource' did exists and a 'body' is send.
	res->status = (exists) ? Response::NO_CONTENT : Response::CREATED;

	if (resource_path.rfind(this->loc_->root, 0) == 0)
	{
		resource_path.erase(0, this->loc_->root.length());
	}

	std::string resource_location = this->loc_->uri + "/" + resource_path;
	String::replaceAll(resource_location, "./", "");
	String::replaceAll(resource_location, "//", "/");

	res->headers["Location"] = resource_location;

	// Append 'response' to the 'queue'
	this->responses_.push(res);
}

void
RequestInterpreter::execDelete(
	const Request & request
)
{
	// https://tools.ietf.org/html/rfc7231#section-4.3.5

	Response * res = this->getDefaultResponse();

	// Delete 'resource'
	try
	{
		Resource::del(RequestInterpreter::getResourcePath(request));
	}
	catch (const std::exception & e)
	{
		Console::log(e.what(), Console::ERROR);
		delete res;
		this->execError(Response::NOT_FOUND, true);
		return ;
	}

	// Update 'response'
	// 202 (Accepted)	: if 'accepted' but not done yet
	// 204 (No Content)	: if 'accepted' and done, but no 'body' is send.
	// 200 (Ok)			: if 'accepted' and done, and a 'body' is send.
	res->status = Response::NO_CONTENT;

	// Append 'response' to the 'queue'
	this->responses_.push(res);
}

void
RequestInterpreter::execConnect(
	const Request & request
)
{
	// https://tools.ietf.org/html/rfc7231#section-4.3.6

	(void) request;

	Response * res = this->getDefaultResponse();

	// Update 'response'
	res->status = Response::OK;

	// A server MUST NOT send any Transfer-Encoding or Content-Length header fields in a 2xx (Successful)
	if (res->status > 199 && res->status < 300)
	{
		res->headers.erase("Transfer-Encoding");
		res->headers.erase("Content-Length");
	}

	// Append 'response' to the 'queue'
	this->responses_.push(res);
}

void
RequestInterpreter::execOptions(
	const Request & request
)
{
	// https://tools.ietf.org/html/rfc7231#section-4.3.7

	(void) request;

	Response * res = this->getDefaultResponse();

	// Update 'response'
	res->status = Response::NO_CONTENT;
	res->headers["Allow"] = "";

	typedef Location::t_method_ctn		t_loc_method_ctn;

	for (t_loc_method_ctn::const_iterator it_method = this->loc_->methods.begin(); it_method != this->loc_->methods.end(); ++it_method)
	{
		if (it_method != this->loc_->methods.begin())
			res->headers["Allow"] += ", ";

		res->headers["Allow"] += Request::methodToStr(*it_method);
	}

	if (res->headers["Allow"].empty())
	{
		res->headers["Allow"] = Request::getKnownMethods();
	}

	// Append 'response' to the 'queue'
	this->responses_.push(res);
}

void
RequestInterpreter::execTrace(
	const Request & request
)
{
	// https://tools.ietf.org/html/rfc7231#section-4.3.8

	(void) request;

	Response * res = this->getDefaultResponse();

	// Update 'response'
	res->status = Response::OK;
	res->headers["Content-Type"] = "message/http";

	// Append 'response' to the 'queue'
	this->responses_.push(res);
}

/**
 * Private getters & setters
 */
void
RequestInterpreter::setServer(const Request &request)
{
	typedef Config::t_server_ctn::const_iterator		srv_iterator;
	typedef Location::t_str_ctn::const_iterator			srv_name_iterator;

	// Get the host to find in the servers list
	URI uri(request.headers.find("Host")->second);

	// Get the server list corresponding to the port
	Config::t_server_ctn servers = this->cfg_.srv_map.find(this->port_)->second;

	// Look for the matching host in server_list
	for (srv_iterator it_srv = servers.begin(); it_srv != servers.end(); ++it_srv)
	{
		Server *tmp_serv = *it_srv;

		// Look for matching names
		for (srv_name_iterator it = tmp_serv->names.begin(); it != tmp_serv->names.end(); ++it)
		{
			// names.begin is address:port so check if host is address or address:port
			if (*it == uri.getHost() || *it == uri.host)
			{
				this->srv_ = tmp_serv;
				return ;
			}
		}
	}

	// Get Default server
	struct sockaddr addr;
	socklen_t len = sizeof(addr);

	if (getsockname(this->conn_->getServer().getFD(), &addr, &len) == -1)
	{
		Console::log("RequestInterpreter::setServer() : getsockname() failed.");
		return ;
	}

	struct sockaddr_in * addr_in = (sockaddr_in *) &addr;
	std::string server_ip = Network::addrToStr(addr_in->sin_addr.s_addr);

	// Console::log(server_ip, Console::ERROR);

	for (srv_iterator it_srv = servers.begin(); it_srv != servers.end(); ++it_srv)
	{
		Server *tmp_serv = *it_srv;

		// Look for matching names
		for (srv_name_iterator it = tmp_serv->names.begin(); it != tmp_serv->names.end(); ++it)
		{
			// names.begin is address:port so check if host is address or address:port
			if (*it == server_ip)
			{
				this->srv_ = tmp_serv;
				return ;
			}
		}
	}

	Console::log("RequestInterpreter::setServer() : Server not found.", Console::ERROR);
}

void
RequestInterpreter::setLocation(const Request &request)
{
	// Server is not defined
	if (!this->srv_)
		return ;

	typedef Server::t_loc_ctn::const_iterator 	loc_iterator;

	URI		uri(request.uri);
	size_t	depth = 0;

	// Check if no uri path is given in request
	if (uri.path.empty())
	{
		Console::log("RequestInterpreter::getLocation() : Location not found.", Console::ERROR);
		return ;
	}

	// Try each 'location' path
	for (loc_iterator it_loc = this->srv_->locations.begin(); it_loc != this->srv_->locations.end(); ++it_loc)
	{
		std::string loc_path = it_loc->first;
		size_t		ret;

		if (loc_path == "/")
			this->loc_ = it_loc->second;
		else if ((ret = this->comparePath(uri.path, it_loc->first)) > depth)
		{
			depth = ret;
			this->loc_ = it_loc->second;
		}
	}

	if (!this->loc_)
		Console::log("RequestInterpreter::getLocation() : Location not found.", Console::ERROR);
}

std::string
RequestInterpreter::getResourcePath(
	const Request & request
) const
{
	URI			uri(request.uri);
	std::string	resource = uri.path;

	// Remove 'location'
	if (resource.rfind(this->loc_->uri, 0) == 0)
	{
		resource.erase(0, this->loc_->uri.length());
	}

	// Prepend 'root' / 'upload_dir'
	if (request.method == "PUT")
	{
		// Is 'upload_dir' absolute or relative  ?
		if (!this->loc_->upload_dir.empty() && this->loc_->upload_dir[0] == '/')
			resource = this->loc_->upload_dir + "/" + resource;
		else
			resource = this->loc_->root + "/" + this->loc_->upload_dir + "/" + resource;
	}
	else
	{
		resource = this->loc_->root + "/" + resource;
	}

	// Remove possible '//'
	String::replaceAll(resource, "//", "/");

	return resource;
}

size_t
RequestInterpreter::comparePath(std::string uri, std::string path) const
{
	std::vector<std::string>		v_path;
	size_t		depth = 0;
	size_t		index = 0;

	// Parse path into a vector to compare with uri
	size_t		ret;
	while ((ret = path.find('/')) != std::string::npos)
	{
		std::string temp = path.substr(0, ret);
		if (temp.size())
			v_path.push_back(temp);
		path.erase(0, ret + 1);
	}
	if (path.size())
		v_path.push_back(path);

	// Compare each v_path with uri part
	while ((ret = uri.find('/')) != std::string::npos)
	{
		std::string temp = uri.substr(0, ret);
		if (temp.empty())
			;
		else if (index < v_path.size() && temp == v_path[index])
			depth += temp.size();
		else
			return depth;
		uri.erase(0, ret + 1);
		if (temp.size())
			index++;
	}
	if (uri.size() && index < v_path.size() && uri == v_path[index])
		depth += uri.size();

	return depth;
}