/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionManager.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/28 00:28:57 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/22 17:49:20 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONNECTION_MANAGER_HPP
# define HTTP_CONNECTION_MANAGER_HPP

# include <list>
# include "Connection.hpp"

class ConnectionManager
{
public:
	// Static const
	static const int MAX_CONN;

	// Typedef
	typedef std::list<Connection *>		t_connection_ctn;

private:
	// Attributes
	t_connection_ctn	connections_;

public:
	// Constructors & Destructor
	ConnectionManager();
	~ConnectionManager();

	// Methods
	void manage(Connection * connection);
	void close(Connection * connection);
	void closeAll();

	// Getters & Setters
	const t_connection_ctn & getConnections() const;

private:
	// C++98 Delete
	ConnectionManager(const ConnectionManager & other);
	ConnectionManager & operator=(const ConnectionManager & other);

}; // -- class ConnectionManager

#endif
