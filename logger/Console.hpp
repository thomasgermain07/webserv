/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Console.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/18 14:38:49 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/07 21:04:24 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_CONSOLE_HPP
# define LOGGER_CONSOLE_HPP

# include <iostream>
# include <string>

class Console
{
public:
	// Severity
	enum eSeverity {
		INFO,
		WARNING,
		ERROR,
		DEBUG
	};

public:
	// Constructors & Destructor
	~Console();

	// Methods
	template <typename T>
	static void
	log(
		const T & message,
		eSeverity severity = DEBUG
	)
	{
		std::cout
			<< getColor(severity)
			<< "[ " << getSeverity(severity) << " ] "
			<< message
			<< "\e[0m"
			<< std::endl;
	}

private:
	// Private methods
	static const std::string getSeverity(Console::eSeverity severity);
	static const std::string getColor(Console::eSeverity severity);

private:
	// C++98 Delete
	Console();
	Console(const Console & other);
	Console & operator=(const Console & other);

}; // -- class Console

#endif
