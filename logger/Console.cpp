/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Console.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/15 22:55:32 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/07 21:04:15 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Console.hpp"

/**
 * Constructors & Destructors
 */
Console::~Console()
{
	// Empty
}

/**
 * Private methods
 */
const std::string
Console::getSeverity(
	Console::eSeverity severity
)
{
	return
		  (severity == INFO)	? "INFO"
		: (severity == WARNING)	? "WARNING"
		: (severity == ERROR)	? "ERROR"
		: (severity == DEBUG)	? "DEBUG"
		: "UNKNOWN"
	;
}

const std::string
Console::getColor(
	Console::eSeverity severity
)
{
	return
		  (severity == INFO)	? "\e[94m"	// FG: Blue
		: (severity == WARNING)	? "\e[93m"	// FG: Yellow
		: (severity == ERROR)	? "\e[91m"	// FG: Red
		: (severity == DEBUG)	? "\e[100;30m"	// FG: Black - BG: Grey
		: "\e[39m"	// FG: Default
	;
}