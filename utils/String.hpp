/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/07 18:24:39 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/11 01:31:01 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_STRING_HPP
# define UTILS_STRING_HPP

# include <string>

class String
{
public:
	// Static
	static void trim(std::string & str, const std::string & characters = " \r\n\t\v\f");
	static void replaceAll(std::string & str, const std::string & from, const std::string & to);
	static void toUpper(std::string & str);

	static std::string getExtension(const std::string & str);

	static char * duplicate(const std::string & str);
	static void del(char * p_str);

};

#endif
