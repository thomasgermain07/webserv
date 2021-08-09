/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Authenticator.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thgermai <thgermai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 21:35:49 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/24 17:39:20 by thgermai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <fstream>
#include "logger/Console.hpp"
#include "utils/Base64.hpp"
#include "Types.hpp"
#include "Authenticator.hpp"

/**
 * Static
 */
void
Authenticator::authenticate(
	const std::string & htpasswd,
	const std::string & credentials
)
{
	// Open file
	std::ifstream file(htpasswd.c_str());

	if (!file.is_open())
	{
		throw std::runtime_error("Authenticator::authenticate() : 'open()' failed.");
	}

	// Check 'credentials'
	bool valid = false;

	std::string line;
	while (getline(file, line))
	{
		// Console::log("Authenticator::authenticate() : " + credentials + " == " + Base64::decode(credentials) + " ?? " + Base64::encode(line) + " == " + line);

		if (credentials == Base64::encode(line))
		{
			valid = true;
			break ;
		}
	}

	// Close file
	file.close();

	// Invalid 'credentials'
	if (!valid)
	{
		throw std::runtime_error("Authenticator::authenticate() : Invalid credentials.");
	}
}
