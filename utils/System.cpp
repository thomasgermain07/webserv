/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   System.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/10 17:12:48 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/13 18:35:15 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>
#include "System.hpp"

/**
 * Static
 */
bool
System::isFile(
	const std::string & path
)
{
	struct stat s;

	if (stat(path.c_str(), &s) == -1)
	{
		// Console::log("System::isFile() : 'stat()' failed.", Console::ERROR);
		return false;
	}

	return S_ISREG(s.st_mode);
}

bool
System::isFolder(
	const std::string & path
)
{
	struct stat s;

	if (stat(path.c_str(), &s) == -1)
	{
		// Console::log("System::isFolder() : 'stat()' failed.", Console::ERROR);
		return false;
	}

	return S_ISDIR(s.st_mode);
}

bool
System::makeFolder(
	const std::string & path
)
{
	if (System::isFolder(path))
		return true;

	std::string::size_type slash = path.rfind("/");

	if (slash != std::string::npos)
	{
		std::string parent_path = path.substr(0, slash);

		if (!System::makeFolder(parent_path))
			return false;
	}

	if (!path.empty() && mkdir(path.c_str(), 0777) == -1)
		return false;

	return true;
}