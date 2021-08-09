/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Authenticate.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/17 17:25:47 by thgermai          #+#    #+#             */
/*   Updated: 2021/01/17 17:25:55 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTHENTICATE_HPP
# define AUTHENTICATE_HPP

# include <iostream>
# include <string>

struct		Authenticate
{
	bool				active;
	std::string			file_path;
	std::string			auth_name;
};

#endif