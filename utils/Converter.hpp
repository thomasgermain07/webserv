/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Converter.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/11 17:56:09 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/14 16:34:50 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_CONVERTER_HPP
# define UTILS_CONVERTER_HPP

# include <sstream>
# include <string>

class Converter
{
public:
	// Static
	template<typename T>
	static T
	strToNumber(
		const std::string & str
	)
	{
		std::stringstream ss;
		T number;

		ss << str;
		ss >> number;

		return number;
	}

	template<typename T>
	static std::string
	numberToStr(
		T number
	)
	{
		std::stringstream ss;

		ss << number;

		return ss.str();
	}

	template<typename T>
	static T
	hexToNumber(
		const std::string & str
	)
	{
		std::stringstream ss;
		T number;

		ss << std::hex << str;
		ss >> number;

		return number;
	}

private:
	// C++98 Delete
	Converter();
	Converter(const Converter & other);
	Converter & operator=(const Converter & other);

}; // -- class Converter

#endif
