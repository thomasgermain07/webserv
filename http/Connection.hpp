/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/27 18:10:44 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/25 16:26:21 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONNECTION_HPP
# define HTTP_CONNECTION_HPP

# include <queue>
# include <netinet/in.h>
# include "Types.hpp"
# include "config/Config.hpp"
# include "utils/Time.hpp"
# include "Socket.hpp"
# include "RequestInterpreter.hpp"
# include "RequestParser.hpp"

class Connection
{
private:
	// Static
	static const size_t	BUF_SIZE;

private:
	// Attributes
	const Socket &		server_socket_;
	t_fd				socket_fd_;
	sockaddr_in			socket_addr_;
	RequestParser		parser_;
	RequestInterpreter	interpreter_;
	std::string			buffer_;
	Time::t_time		timeout_;
	bool				accepted_;

public:
	// Constructor & Destructor
	Connection(t_fd socket_fd, sockaddr_in & socket_addr, const Config & cfg, const Socket & server);
	~Connection();

	// Methods
	void do_read();
	void do_write();

	void close();

	// Getters & Setters
	bool isInactive() const;
	t_fd getSocketFD() const;
	std::string getIP() const;
	const Socket & getServer() const;

	void setAccepted(bool accepted);

private:
	// Private methods
	void updateTimeout();

private:
	// C++98 delete
	Connection();
	Connection(const Connection & other);
	Connection & operator=(const Connection & other);

}; // -- class Connection

#endif
