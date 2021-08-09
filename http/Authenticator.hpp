/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Authenticator.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 21:31:10 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/27 14:50:12 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_AUTHENTICATOR_HPP
# define HTTP_AUTHENTICATOR_HPP

# include <string>

class Authenticator
{
public:
	// Static
	static void authenticate(const std::string & htpasswd, const std::string & credentials);
};

#endif
