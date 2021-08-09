/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/30 14:55:26 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/09 00:35:20 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEB_SERVER_HPP
# define WEB_SERVER_HPP

# include <vector>

# include "config/Config.hpp"
# include "http/ConnectionManager.hpp"
# include "http/Socket.hpp"

class WebServer
{
public:
	// Static
	static bool running;

	// Typedef
	typedef std::vector<Socket *>		t_socket_ctn;

private:
	// Attributes
	const Config &		cfg_;
	ConnectionManager 	cm_;
	t_socket_ctn		sockets_;
	fd_set				master_set_;

public:
	// Constructors & Destructor
	WebServer(const Config & cfg);
	~WebServer();

	// Methods
	void run();

private:
	// Private methods
	void start();
	void loop();
	void stop();

private:
	// C++98 delete
	WebServer();
	WebServer(const WebServer & other);
	WebServer & operator=(const WebServer & other);
};

#endif