/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/07 18:28:26 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 18:30:30 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "String.hpp"

/**
 * Static
 */
void
String::trim(
	std::string & str,
	const std::string & characters
)
{
	std::string::size_type first = str.find_first_not_of(characters);
	std::string::size_type last = str.find_last_not_of(characters);

	// Leading 'characters'
	str.erase(0, first);

	// Trailing 'characters'
	if (last != std::string::npos)
		str.erase(last + 1);
}

void
String::replaceAll(
	std::string & str,
	const std::string & from,
	const std::string & to
)
{
	std::string::size_type	found;

	while ((found = str.find(from)) != std::string::npos)
	{
		str.replace(found, from.length(), to);
	}
}

void
String::toUpper(
	std::string & str
)
{
	for (std::string::size_type pos = 0; pos < str.length(); ++pos)
	{
		if (std::islower(str[pos]))
			str[pos] = std::toupper(str[pos]);
	}
}


std::string
String::getExtension(
	const std::string & str
)
{
	std::string ext;
	std::string::size_type found;

	if ((found = str.rfind(".")) != std::string::npos)
		ext = str.substr(found);

	return ext;
}

char *
String::duplicate(
	const std::string & str
)
{
	char * copy = new char[str.length() + 1];

	std::string::size_type i;
	for (i = 0; i < str.length(); ++i)
		copy[i] = str[i];
	copy[i] = 0;

	return copy;
}

void
String::del(char * p_str)
{
	delete[] p_str;
}