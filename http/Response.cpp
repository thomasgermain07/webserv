/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/18 14:00:19 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 20:25:36 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "logger/Console.hpp"
#include "Request.hpp"
#include "Response.hpp"

/**
 * Static
 */
std::string
Response::statusToStr(Response::eStatus status)
{
	switch (status)
	{
		// Informational (1xx)
		case CONTINUE							: return "Continue";
		case SWITCHING_PROTOCOLS				: return "Switching Protocols";
		case PROCESSING							: return "Processing";
		case EARLY_HINTS						: return "Early Hints";
		// Success (2xx)
		case OK									: return "Ok";
		case CREATED							: return "Created";
		case ACCEPTED							: return "Accepted";
		case NON_AUTHORITATIVE_INFORMATION		: return "Non-Authoritative Information";
		case NO_CONTENT							: return "No Content";
		case RESET_CONTENT						: return "Reset Content";
		case PARTIAL_CONTENT					: return "Partial Content";
		case MULTI_STATUS						: return "Multi-Status";
		case ALREADY_REPORTED					: return "Already Reported";
		case IM_USED							: return "IM Used";
		// Redirection (3xx)
		case MULTIPLE_CHOICES					: return "Multiple Choice";
		case MOVED_PERMANENTLY					: return "Moved Permanently";
		case FOUND								: return "Found";
		case SEE_OTHER							: return "See Other";
		case NOT_MODIFIED						: return "Not Modified";
		case USE_PROXY							: return "Use Proxy";
		case SWITCH_PROXY						: return "Switch Proxy";
		case TEMPORARY_REDIRECT					: return "Temporary Redirect";
		case PERMANENT_REDIRECT					: return "Permanent Redirect";
		// Client Errors (4xx)
		case BAD_REQUEST						: return "Bad Request";
		case UNAUTHORIZED						: return "Unauthorized";
		case PAYMENT_REQUIRED					: return "Payment Required";
		case FORBIDDEN							: return "Forbidden";
		case NOT_FOUND							: return "Not Found";
		case METHOD_NOT_ALLOWED					: return "Method Not Allowed";
		case NOT_ACCEPTABLE						: return "Not Acceptable";
		case PROXY_AUTHENTICATION_REQUIRED		: return "Proxy Authentication Required";
		case REQUEST_TIMEOUT					: return "Request Timeout";
		case CONFLICT							: return "Conflict";
		case GONE								: return "Gone";
		case LENGTH_REQUIRED					: return "Length Required";
		case PRECONDITION_FAILED				: return "Precondition Failed";
		case PAYLOAD_TOO_LARGE					: return "Payload Too Large";
		case URI_TOO_LONG						: return "URI Too Long";
		case UNSUPPORTED_MEDIA_TYPE				: return "Unsupported Media Type";
		case RANGE_NOT_SATISFIABLE				: return "Range Not Satisfiable";
		case EXPECTATION_FAILED					: return "Expectation Failed";
		case IM_A_TEAPOT						: return "I'm a teapot";
		case MISDIRECTED_REQUEST				: return "Misdirected Request";
		case UNPROCESSABLE_ENTITY				: return "Unprocessable Entity";
		case LOCKED								: return "Locked";
		case FAILED_DEPENDENCY					: return "Failed Dependency";
		case TOO_EARLY							: return "Too Early";
		case UPGRADE_REQUIRED					: return "Upgrade Required";
		case PRECONDITION_REQUIRED				: return "Precondition Required";
		case TOO_MANY_REQUEST					: return "Too Many Request";
		case REQUEST_HEADER_FIELDS_TOO_LARGE	: return "Request Header Fields Too Large";
		case UNAVAILABLE_FOR_LEGAL_REASONS		: return "Unavailable For Legal Reasons";
		// Server Errors (5xx)
		case INTERNAL_SERVER_ERROR				: return "Internal Server Error";
		case NOT_IMPLEMENTED					: return "Not Implemented";
		case BAD_GATEWAY						: return "Bad Gateway";
		case SERVICE_UNAVAILABLE				: return "Service Unavailable";
		case GATEWAY_TIMEOUT					: return "Gateway Timeout";
		case HTTP_VERSION_NOT_SUPPORTED			: return "HTTP Version Not Supported";
		case VARIANT_ALSO_NEGOTIATES			: return "Variant Also Negotiates";
		case INSUFFICIENT_STORAGE				: return "Insufficient Storage";
		case LOOP_DETECTED						: return "Loop Detected";
		case NOT_EXTENDED						: return "Not Extended";
		case NETWORK_AUTHENTICATION_REQUIRED	: return "Network Authentication Required";
		default									: return "Undefined";
	}
}

/**
 * Methods
 */
std::string
Response::toString() const
{
	std::stringstream ss;

	// Request-Line
	ss << "HTTP/" << this->http_version_major << "." << this->http_version_minor;
	ss << " " << this->status;
	ss << " " << Response::statusToStr(this->status);
	ss << "\r\n";

	// Headers
	typedef Request::t_headers_ctn		t_h_ctn;

	for (t_h_ctn::const_iterator h = this->headers.begin(); h != this->headers.end(); ++h)
	{
		if (!h->second.empty())
		{
			ss << h->first << ": " << h->second << "\r\n";
		}
	}

	// Console::log("Response: \n" + ss.str());

	// Empty-Line
	ss << "\r\n";

	// Body
	ss << this->body;


	return ss.str();
}
