/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 15:26:41 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/21 16:10:43 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_MIME_HPP
# define HTTP_MIME_HPP

# include <string>

struct Mime
{
	// Static
	static std::string extToStr(const std::string & ext);
};

#endif
