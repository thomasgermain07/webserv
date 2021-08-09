/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Time.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/02 01:16:15 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/14 18:18:34 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_TIME_HPP
# define UTILS_TIME_HPP

# include <ctime>
# include <sys/time.h>
# include <string>

class Time
{
public:
	// Typedef
	typedef time_t		t_time;

	// Static methods
	static t_time getTime();
	static std::string getUTCString();

private:
	// C++98 delete
	Time();
	Time(const Time & other);
	Time & operator=(const Time & other);
};

#endif
