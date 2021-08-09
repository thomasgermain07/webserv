/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:10:51 by tboisnie          #+#    #+#             */
/*   Updated: 2021/02/24 23:44:18 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include "Mime.hpp"

/**
 * Static
 */
std::string
Mime::extToStr(
	const std::string & ext
)
{
	// https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types

	std::map<std::string, std::string> ext_m;

	ext_m[".aac"]	= "audio/aac";
	ext_m[".abw"]	= "application/x-abiword";
	ext_m[".arc"]	= "application/x-freearc";
	ext_m[".avi"]	= "video/x-msvideo";
	ext_m[".azw"]	= "application/vnd.amazon.ebook";
	ext_m[".bin"]	= "application/octet-stream";
	ext_m[".bmp"]	= "image/bmp";
	ext_m[".bz"]	= "application/x-bzip";
	ext_m[".bz2"]	= "application/x-bzip2";
	ext_m[".csh"]	= "application/x-csh";
	ext_m[".css"]	= "text/css";
	ext_m[".csv"]	= "text/csv";
	ext_m[".doc"]	= "application/msword";
	ext_m[".docx"]	= "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	ext_m[".eot"]	= "application/vnd.ms-fontobject";
	ext_m[".epub"]	= "application/epub+zip";
	ext_m[".gz"]	= "application/gzip";
	ext_m[".gif"]	= "image/gif";
	ext_m[".htm"]	= "text/html";
	ext_m[".html"]	= "text/html";
	ext_m[".ico"]	= "image/vnd.microsoft.icon";
	ext_m[".ics"]	= "text/calendar";
	ext_m[".jar"]	= "application/java-archive";
	ext_m[".jpg"]	= "image/jpeg";
	ext_m[".jpeg"]	= "image/jpeg";
	ext_m[".js"]	= "text/javascript";
	ext_m[".json"]	= "application/json";
	ext_m[".jsonld"]	= "application/ld+json";
	ext_m[".mid"]	= "audio/midi";
	ext_m[".midi"]	= "audio/midi";
	ext_m[".mjs"]	= "text/javascript";
	ext_m[".mp3"]	= "audio/mpeg";
	ext_m[".mpeg"]	= "video/mpeg";
	ext_m[".mpkg"]	= "application/vnd.apple.installer+xml";
	ext_m[".odp"]	= "application/vnd.oasis.opendocument.presentation";
	ext_m[".ods"]	= "application/vnd.oasis.opendocument.spreadsheet";
	ext_m[".odt"]	= "application/vnd.oasis.opendocument.text";
	ext_m[".oga"]	= "audio/ogg";
	ext_m[".ogv"]	= "video/ogg";
	ext_m[".ogx"]	= "application/ogg";
	ext_m[".opus"]	= "audio/opus";
	ext_m[".otf"]	= "font/otf";
	ext_m[".png"]	= "image/png";
	ext_m[".pdf"]	= "application/pdf";
	ext_m[".php"]	= "application/x-httpd-php";
	ext_m[".ppt"]	= "application/vnd.ms-powerpoint";
	ext_m[".pptx"]	= "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	ext_m[".rar"]	= "application/vnd.rar";
	ext_m[".rtf"]	= "application/rtf";
	ext_m[".sh"]	= "application/x-sh";
	ext_m[".svg"]	= "image/svg+xml";
	ext_m[".swf"]	= "application/x-shockwave-flash";
	ext_m[".tar"]	= "application/x-tar";
	ext_m[".tif"]	= "image/tiff";
	ext_m[".tiff"]	= "image/tiff";
	ext_m[".ts"]	= "video/mp2t";
	ext_m[".ttf"]	= "font/ttf";
	ext_m[".txt"]	= "text/plain";
	ext_m[".vsd"]	= "application/vnd.visio";
	ext_m[".wav"]	= "audio/wav";
	ext_m[".weba"]	= "audio/webm";
	ext_m[".webm"]	= "video/webm";
	ext_m[".webp"]	= "image/webp";
	ext_m[".woff"]	= "font/woff";
	ext_m[".woff2"]	= "font/woff2";
	ext_m[".xhtml"]	= "application/xhtml+xml";
	ext_m[".xls"]	= "application/vnd.ms-excel";
	ext_m[".xlsx"]	= "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	ext_m[".xml"]	= "text/xml";
	ext_m[".xul"]	= "application/vnd.mozilla.xul+xml";
	ext_m[".zip"]	= "application/zip";
	ext_m[".3gp"]	= "video/3gpp";
	ext_m[".3g2"]	= "video/3gpp2";
	ext_m[".7z"]	= "application/x-7z-compressed";

	if (!ext.empty() && ext_m.count(ext))
		return ext_m[ext];

	return ext_m[".bin"];
}
