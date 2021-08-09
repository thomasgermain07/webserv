/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestInterpreter.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/16 15:58:34 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/19 13:23:28 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_INTERPRETER_HPP
# define REQUEST_INTERPRETER_HPP

# include <queue>
# include <string>
# include "Types.hpp"
# include "config/Config.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Resource.hpp"
# include "Location.hpp"

class Connection;

class RequestInterpreter
{
private:
	// Attributes
	const Config &			cfg_;
	t_port 					port_;
	std::queue<Response *>	responses_;
	const Server *			srv_;
	const Location *		loc_;
	const Connection *		conn_;

public:
	// Constructors & Destructor
	RequestInterpreter(const Config & cfg, t_port port);
	~RequestInterpreter();

	// Methods
	void exec(const Request & request, const Connection * connection);
	void execError(Response::eStatus status, bool body = true);

	// Getters & Setters
	bool hasResponse() const;
	Response * getResponse();

private:
	// Private methods
	Response * getDefaultResponse() const;
	void initLocationFromServer(Location & new_loc);

	void execGet(const Request & request, bool load_content = true);
	void execHead(const Request & request);
	void execPost(const Request & request);
	void execPut(const Request & request);
	void execDelete(const Request & request);
	void execConnect(const Request & request);
	void execOptions(const Request & request);
	void execTrace(const Request & request);

	// Private getters & setters
	void setServer(const Request & request);
	void setLocation(const Request & request);
	std::string getResourcePath(const Request & request) const;
	std::string getUri(const std::string & input) const;

	// Utils functions
	size_t comparePath(std::string uri, std::string path) const;

private:
	// C++98 Delete
	RequestInterpreter(const RequestInterpreter & other);
	RequestInterpreter & operator=(const RequestInterpreter & other);

};

#endif
