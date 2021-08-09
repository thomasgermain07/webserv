/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/22 18:39:19 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 20:40:43 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include "logger/Console.hpp"
#include "utils/System.hpp"
#include "utils/Converter.hpp"
#include "Types.hpp"
#include "Resource.hpp"

/**
 * Static
 */
size_t
Resource::get(
	const std::string & path,
	std::string * body
)
{
	// Console::log("Resource::get() : " + path);

	// File size
	struct stat s;

	if (stat(path.c_str(), &s) == -1)
	{
		throw std::runtime_error("Resource::get() : 'stat()' failed.");
	}

	size_t file_size = static_cast<size_t>(s.st_size);

	if (body)
	{
		body->reserve(file_size);

		// File content
		std::ifstream file(path.c_str());

		if (file.fail())
		{
			throw std::runtime_error("Resource::get() : 'open()' failed.");
		}

		std::string buffer;

		while (std::getline(file, buffer))
		{
			body->append(buffer + "\n");
		}

		body->resize(file_size);

		if (!file.eof())
		{
			file.close();
			throw std::runtime_error("Resource::get() : 'getline()' failed.");
		}

		file.close();
	}

	return file_size;
}

size_t
Resource::getListing(
	const std::string & path,
	const Request & request,
	std::string * body
)
{
	// Console::log("Resource::getListing() : " + path);

	if (body == NULL)
		return 0;

	//  Open 'folder'
	DIR * dir = opendir(path.c_str());
	if (!dir)
	{
		throw std::runtime_error("Resource::getListing() : 'opendir()' failed.");
	}

	// Read 'folder' contents
	std::list<std::string> files;
	dirent * content;
	while ((content = readdir(dir)) != NULL)
	{
		files.push_back(content->d_name);
	}

	closedir(dir);

	std::string file_tag = "<li> <a class=\"link\" href=\"/$link$\">$file_name$</a> </li>";
	std::string html_body = "<html> <head>  <title>WebServ</title>  <style>   *, *::after, *::before {    margin:0;    padding:0;    box-sizing: border-box;   }   body {    display: flex;    align-items: center;    justify-content: center;    color: #363333;    font-family: sans-serif;    background: #d5d5d5;   }   .window {    width: 50vw;    min-width: 600px;    border-radius: 10px;    box-shadow:   0 20px 25px 0 #363333;    -moz-box-shadow: 0 20px 25px 0 #363333;    -webkit-box-shadow: 0 20px 25px 0 #363333;    overflow: hidden;   }   .window__header {    padding: 10px 0 7px;    position: relative;    background: #363333;   }   .btn {    display: inline-block;    width: 15px;    height: 15px;    border-radius: 50%;    position: absolute;    top: 50%;    transform: translateY(-50%);   }   .btn--red  {left: 15px; background: #da2d2d;}   .btn--yellow {left: 45px; background: #e69f34;}   .btn--green  {left: 75px; background: #51c734;}   h1 {    margin: 0 100px;    color: #d5d5d5;    font-size: 20px;    text-align: center;   }   .window__content {    display: grid;    grid-template-columns: 200px 1fr;    grid-column-gap: 15px;    padding: 30px;   }   .link {    font-family: Tahoma, sans-serif;   }   .link:link {    color: #363333;    text-decoration: none;   }   .link:hover {    color: whitesmoke;    text-decoration-line: underline;    text-decoration-color: whitesmoke;   }  </style> </head> <body>  <div class=\"window\">   <header class=\"window__header\">    <i class=\"btn btn--red\"></i>    <i class=\"btn btn--yellow\"></i>    <i class=\"btn btn--green\"></i>    <h1>WebServ - Tboisnie - Thgermai</h1>   </header>   <div class=\"window__content\">    <ul style=\"list-style-type: disc;\">";
	std::string html_body_2 = "</ul> </div> </div> </body> </html>";

	for (std::list<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		std::string temp = file_tag;
		std::string link = request.uri + "/" + *it;

		// Remove possible double '/' when joinning
		size_t ret;
		if ((ret = link.find("//")) != std::string::npos)
			link.erase(ret, 1);

		// Remove the first '/'
		if (link[0] == '/')
			link.erase(0, 1);

		ret = temp.find("$link$");
		temp.replace(ret, 6, link);

		ret = temp.find("$file_name$");
		temp.replace(ret, 11, *it);

		html_body.append(temp);
	}

	html_body.append(html_body_2);

	body->append(html_body);

	return html_body.size();
}

size_t
Resource::getError(
	const Config::t_err_ctn & err_map,
	const std::string & root,
	std::string * body,
	Response::eStatus err
)
{
	if (body == NULL)
		return 0;

	// Find code into the err_map
	Config::t_err_ctn::const_iterator it = err_map.find(err);

	// If no match return generateErrorPage
	if (it == err_map.end())
		return Resource::generateErrorPage(err, body);

	std::string path = it->second;

	// If path isn't a file, test with root
	if (System::isFile(path) == false)
	{
		if (path[0] != '/')
		{
			path = root + "/" + path;

			// Delete '//'
			size_t ret;
			if ((ret = path.find("//")) != std::string::npos)
				path.erase(ret, 1);
		}

		// Re-check with path + file
		if (System::isFile(path) == false)
		{
			Console::log("Resource::getError() : path is invalid : " + path, Console::WARNING);
			return Resource::generateErrorPage(err, body);
		}
	}

	return Resource::get(path, body);
}

void
Resource::save(
	const std::string & path,
	const std::string & content
)
{
	t_fd resource;

	// Console::log("Resource::save() : " + path);

	// Sub-folders creation
	size_t slash = path.rfind("/");

	if (slash != std::string::npos)
	{
		if (!System::makeFolder(path.substr(0, slash)))
			throw std::runtime_error("Resource::save() : Path folders creation failed.");
	}

	// File creation
	if (slash == std::string::npos || !path.substr(slash + 1).empty())
	{
		if ((resource = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		{
			throw std::runtime_error("Resource::save() : 'open()' failed.");
		}

		if (write(resource, content.c_str(), content.length()) == -1)
		{
			throw std::runtime_error("Resource::save() : 'write()' failed.");
		}

		close(resource);
	}
}

void
Resource::del(
	const std::string & path
)
{
	// Console::log("Resource::del() : " + path);

	if (!System::isFile(path))
	{
		throw std::runtime_error("Resource::del() : File not found.");
	}

	unlink(path.c_str());
}

size_t
Resource::generateErrorPage(
	Response::eStatus err,
	std::string * body
)
{
	// Console::log("Resource::generateErrorPage() : Generating a custom page.");

	std::string html_body ="<html>" \
		"<head>" \
			"<title>Error $code$</title>" \
			"<style>" \
				"*,*::after,*::before{margin:0;padding:0;box-sizing:border-box;}" \
				"body{display:flex;align-items:center;justify-content:center;color:#363333;font-family:sans-serif;background:#d5d5d5;}" \
				".window{width:50vw;min-width:600px;border-radius:10px;box-shadow:0 20px 25px 0 #363333;-moz-box-shadow: 0 20px 25px 0 #363333;-webkit-box-shadow:0 20px 25px 0 #363333;overflow:hidden;}" \
				".window__header{padding:10px 0 7px 0;position:relative;background:#363333;}" \
				".btn{display:inline-block;width:15px;height:15px;border-radius:50%; position:absolute;top:50%;transform:translateY(-50%);}" \
				".btn--red{left: 15px;background:#da2d2d;}.btn--yellow{left:45px;background:#e69f34;}.btn--green{left:75px;background:#51c734;}" \
				"h1{margin:0 100px;color:#d5d5d5;font-size: 20px;text-align:center;}.window__content{display:grid;grid-template-columns:200px 1fr;grid-column-gap:15px;padding:30px;}" \
				".logo{width:200px;height:200px;border:8px solid #363333;border-radius: 50%; position:relative;}" \
				".logo__eye{width:25px;height:25px;border-radius:50%; position:absolute;top:50px;background:#363333;}" \
				".logo__eye--left{left:40px;}.logo__eye--right{right:40px;}" \
				".logo__mouth{width:55%; height:10px;border-radius:5px;position:absolute;bottom:50px;left:50%;transform:translateX(-50%) rotateZ(-5deg);background:#363333;}" \
				".error__code{margin:30px 0;text-align:center;font-size:48px;font-weight:600;}" \
				".error__msg{text-align:center;font-size:24px;}" \
			"</style>" \
		"</head>" \
		"<body>" \
			"<div class='window'>" \
				"<header class='window__header'>" \
					"<i class='btn btn--red'></i><i class='btn btn--yellow'></i><i class='btn btn--green'></i>" \
					"<h1>WebServ - Tboisnie - Thgermai</h1>" \
				"</header>" \
				"<div class='window__content'>" \
					"<div class='logo'>" \
						"<div class='logo__eye logo__eye--left'></div><div class='logo__eye logo__eye--right'></div>" \
						"<div class='logo__mouth'></div>" \
					"</div>" \
					"<div>" \
						"<div class='error__code'>Error $code$</div>" \
						"<div class='error__msg'>$message$</div>" \
					"</div>" \
				"</div>" \
			"</div>" \
		"</body>" \
	"</html>";

	std::string code_str = Converter::numberToStr(err);
	std::string code_msg = Response::statusToStr(err);

	size_t ret;
	while ((ret = html_body.find("$code$")) != std::string::npos)
		html_body.replace(ret, 6, code_str);

	while ((ret = html_body.find("$message$")) != std::string::npos)
		html_body.replace(ret, 9, code_msg);

	body->append(html_body);

	return html_body.size();
}
