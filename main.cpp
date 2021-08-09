/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/15 15:19:03 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/25 22:13:02 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <signal.h>
#include "config/Config.hpp"
#include "config/ConfigParser.hpp"
#include "logger/Console.hpp"
#include "WebServer.hpp"

void
handleSignal(
	int sig
)
{
	switch (sig)
	{
		case SIGINT:
			WebServer::running = false;
			break ;
		default:
			break ;
	}
}

int
main(int ac, char *av[], char *envp[])
{
	if (ac >= 3)
	{
		std::cout << "Usage: ./webserv 'path/config.conf'" << std::endl;
		return 1;
	}

	if (signal(SIGINT, handleSignal) == SIG_ERR)
	{
		Console::log("main : 'signal()' failed.", Console::ERROR);
		return 1;
	}

	// Check default 'config' file path.
	std::string cfg_filepath = (ac == 2) ? av[1] : "./test/42/42mac.conf";

	try
	{
		// Parse file
		ConfigParser	cfg_parser(cfg_filepath, envp);

		// Get 'config'
		const Config & cfg = cfg_parser.getConfig();

		try
		{
			// Start 'WebServ'
			WebServer ws(cfg);
			ws.run();
		}
		catch (const std::exception & ws_e)
		{
			Console::log(ws_e.what(), Console::ERROR);
		}
	}
	catch (const std::exception & cfg_e)
	{
		Console::log(cfg_e.what(), Console::ERROR);
	}

	return 0;
}
