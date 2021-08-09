/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base64.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/27 17:25:43 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/27 17:27:32 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_BASE64_HPP
# define UTILS_BASE64_HPP

# include <string>

class Base64
{
public:
	// Static
	static std::string encode(const std::string & str);
	static std::string decode(const std::string & str);
};

#endif
