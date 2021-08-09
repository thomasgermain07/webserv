/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   System.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/10 17:11:07 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/13 16:36:39 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_SYSTEM_HPP
# define UTILS_SYSTEM_HPP

# include <string>

class System
{
public:
	// Static
	static bool isFile(const std::string & path);
	static bool isFolder(const std::string & path);

	static bool makeFolder(const std::string & path);

private:
	// c++98 Delete
	System();
	System(const System & other);
	System & operator=(const System & other);
};

#endif
