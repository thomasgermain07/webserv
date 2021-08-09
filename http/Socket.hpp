/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/28 00:18:34 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/25 17:49:05 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SOCKET_HPP
# define HTTP_SOCKET_HPP

# include "config/Config.hpp"
# include "Types.hpp"

class ConnectionManager;

class Socket
{
private:
	// Attributes
	t_fd				fd_;
	t_port				port_;
	ConnectionManager &	cm_;
	const Config &		cfg_;

public:
	// Constructors & Destructor
	Socket(ConnectionManager & cm, const Config & cfg);
	~Socket();

	// Methods
	void start(in_addr_t addr, t_port port);
	void stop();

	t_fd acceptConnection();

	// Getters & Setters
	t_fd getFD() const;
	t_port getPort() const;

private:
	// C++98 Delete
	Socket();
	Socket(const Socket & other);
	Socket & operator=(const Socket & other);

}; // -- class Socket

#endif
