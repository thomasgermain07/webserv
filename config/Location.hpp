/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/27 21:22:37 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 19:02:25 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_LOCATION_HPP
# define CONFIG_LOCATION_HPP

# include "Authenticate.hpp"
# include "Types.hpp"
# include "Request.hpp"
# include "Config.hpp"

struct Location
{
	// Typedef
	typedef Config::t_cgi_ctn				t_cgi_ctn;
	typedef Config::t_loc_ctn				t_loc_ctn;
	typedef Config::t_err_ctn				t_err_ctn;
	typedef Config::t_str_ctn				t_str_ctn;
	typedef Config::t_method_ctn			t_method_ctn;

	// Exclusif Attributes
	t_method_ctn			methods;
	std::string				uri;

	// Commun Attributes
	std::string				root;
	size_t					client_max_body_size;
	Authenticate			authenticate;
	t_err_ctn				error_pages;
	t_str_ctn				indexes;
	bool					auto_index;
	std::string				upload_dir;
	t_cgi_ctn				cgi; // extension : executable

	// Construtor
	Location();
};

#endif
