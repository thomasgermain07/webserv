/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/10 15:31:34 by tboisnie          #+#    #+#             */
/*   Updated: 2021/03/01 20:27:20 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <algorithm>
#include "utils/Converter.hpp"
#include "logger/Console.hpp"
#include "RequestParser.hpp"

// BWS		= OWS
//			; "bad" whitespace
// CR		=  %x0D
//			; carriage return
// CRLF		=  CR LF
//			; Internet standard newline
// CTL		=  %x00-1F / %x7F
//			; controls
// DIGIT	=  %x30-39
//			; 0-9
// DQUOTE	=  %x22
//			; " (Double Quote)
// HEXDIG	=  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
// HTAB		=  %x09
//			; horizontal tab
// LF		=  %x0A
//			; linefeed
// LWSP		=  *(WSP / CRLF WSP)
//			; Use of this linear-white-space rule permits lines containing only
//			; white space that are no longer legal in mail headers and have
//			; caused interoperability problems in other contexts.
//			; Do not use when defining mail headers and use with caution in
//			; other contexts.
// OCTET	=  %x00-FF
//			; 8 bits of data
// OWS		= *( SP / HTAB )
//			; optional whitespace
// RWS		= 1*( SP / HTAB )
//			; required whitespace
// SP		=  %x20
// VCHAR	=  %x21-7E
//			; visible (printing) characters
// WSP		=  SP / HTAB
//			; white space

/**
 * Constructors & Destructor
 */
RequestParser::RequestParser()
	: continue_(true)
	, error_(Response::OK)
	, state_(REQUEST_LINE)
	, req_()
{
	// Empty
}

RequestParser::~RequestParser()
{
	while (!this->requests_.empty())
	{
		delete this->requests_.front();
		this->requests_.pop();
	}

	if (this->req_)
		delete this->req_;
}

/**
 * Methods
 */
void
RequestParser::parse(
	std::string & raw_request
)
{
	// HTTP-message	= start-line
	//				*( header-field CRLF )
	//				CRLF
	//				[ message-body ]

	if (this->hasError())
		return ;

	this->continue_ = true;
	while (this->continue_)
	{
		switch (this->state_)
		{
			case REQUEST_LINE:
				this->parseRequestLine(raw_request);
				break;
			case HEADER:
				this->parseHeader(raw_request);
				break;
			case EMPTY_LINE:
				this->parseEmptyLine(raw_request);
				break;
			case BODY:
				this->parseBody(raw_request);
				break;
			case CHUNKED_BODY:
				this->parseChunkedBody(raw_request);
				break;
			case COMPLETE:
				this->parseComplete();
				break;
			default:
				break;
		}
	}
}

/**
 * Getters & Setters
 */
bool
RequestParser::hasRequest() const
{
	return !this->requests_.empty();
}

Request *
RequestParser::getRequest()
{
	if (this->requests_.empty())
		return 0; // nullptr

	Request * ret = this->requests_.front();
	this->requests_.pop();

	return ret;
}

bool
RequestParser::hasError() const
{
	return (this->error_ != Response::OK);
}

Response::eStatus
RequestParser::getError() const
{
	return this->error_;
}


/**
 * Private methods
 */
void
RequestParser::parseRequestLine(
	std::string & raw_request
)
{
	// request-line		= method SP request-target SP HTTP-version CRLF
	// method			= token
	// token			= 1*tchar
	// tchar			= "!" / "#" / "$" / "%" / "&" / "'" / "*"
	//					/ "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
	//					/ DIGIT / ALPHA
	//					; any VCHAR, except delimiters
	// request-target	= origin-form
	//					/ absolute-form
	//					/ authority-form
	//					/ asterisk-form
	// origin-form		= absolute-path [ "?" query ]
	// absolute-form	= absolute-URI
	// authority-form	= authority
	// asterisk-form	= "*"
	// HTTP-version		= HTTP-name "/" DIGIT "." DIGIT
	// HTTP-name		= %x48.54.54.50 ; "HTTP", case-sensitive

	if (!this->req_)
		this->req_ = new Request();

	std::string::size_type end_pos = raw_request.find("\r\n");

	// Ignore leading 'empty line'
	if (end_pos == 0)
	{
		raw_request.erase(0, 2);
		return ;
	}

	if (end_pos == std::string::npos)
	{
		this->continue_ = false;
		return ;
	}

	// Copy 'request' line
	std::string line = raw_request.substr(0, end_pos);

	// Console::log("RequestParser::parseRequestLine() : `" + line + "`");

	// Remove 'request' line from 'raw request' (buffer)
	raw_request.erase(0, end_pos + 2);

	// Check 'method'
	end_pos = 0;
	while (end_pos < line.length() && this->isTchar(line[end_pos]) )
		++end_pos;

	if (end_pos == 0)
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}

	this->req_->method = line.substr(0, end_pos);
	line.erase(0, end_pos);

	// Check 'SP'
	if (line.empty() || !this->isWsp(line[0]))
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}
	line.erase(0, 1);

	// Check 'request-target'
	// TODO : Parse with RFC 3986 specifications
	end_pos = 0;
	while (end_pos < line.length() && !this->isWsp(line[end_pos]))
		++end_pos;

	if (end_pos == 0)
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}

	// if (end_pos > 2048)
	// {
	// 	this->error_ = Response::URI_TOO_LONG;
	// 	this->continue_ = false;
	// 	return ;
	// }

	this->req_->uri = line.substr(0, end_pos);
	line.erase(0, end_pos);

	// Check 'SP'
	if (line.empty() || !this->isWsp(line[0]))
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}
	line.erase(0, 1);

	// Check 'HTTP-version'
	if (line.find("HTTP/") != 0)
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}
	line.erase(0, 5);

	// Check 'HTTP-version' major
	end_pos = 0;
	while (end_pos < line.length() && this->isDigit(line[end_pos]))
		++end_pos;

	if (end_pos == 0)
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}

	this->req_->http_version_major = Converter::strToNumber<int>(line.substr(0, end_pos));
	line.erase(0, end_pos);

	// Check 'HTTP-version' dot
	if (line.find(".") != 0)
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}
	line.erase(0, 1);

	// Check 'HTTP-version' minor
	end_pos = 0;
	while (end_pos < line.length() && this->isDigit(line[end_pos]))
		++end_pos;

	if (end_pos == 0)
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}

	this->req_->http_version_minor = Converter::strToNumber<int>(line.substr(0, end_pos));
	line.erase(0, end_pos);

	// Invalid trailing caracters
	if (!line.empty())
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}

	this->state_ = HEADER;
}

void
RequestParser::parseHeader(
	std::string & raw_request
)
{
	// header-field		= field-name ":" OWS field-value OWS
	// field-name		= token
	// field-value		= *( field-content )
	// field-content	= field-vchar [ 1*( SP / HTAB ) field-vchar ]
	// field-vchar		= VCHAR
	// token			= 1*tchar
	// tchar			= "!" / "#" / "$" / "%" / "&" / "'" / "*"
	//					/ "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
	//					/ DIGIT / ALPHA
	//					; any VCHAR, except delimiters

	std::string::size_type end_pos = raw_request.find("\r\n");

	if (end_pos == std::string::npos)
	{
		this->continue_ = false;
		return ;
	}

	// Check if 'empty line'
	if (end_pos == 0)
	{
		this->state_ = EMPTY_LINE;
		return ;
	}

	// Copy 'header' line
	std::string line = raw_request.substr(0, end_pos);

	// Console::log("RequestParser::parseHeader() : `" + line + "`");

	// Remove 'header' line from 'raw request' (buffer)
	raw_request.erase(0, end_pos + 2);

	// Check 'header' name
	std::string header_name;

	end_pos = 0;
	while (end_pos < line.length() && this->isTchar(line[end_pos]))
		++end_pos;

	header_name = line.substr(0, end_pos);
	line.erase(0, end_pos);

	// Check ':'
	if (line.find(":") != 0)
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
		return ;
	}
	line.erase(0, 1);

	// Check OWS
	if (!line.empty() && this->isWsp(line[0]))
		line.erase(0, 1);

	// Check 'header' value
	std::string header_value;

	end_pos = 0;
	while (end_pos < line.length() && this->isVchar(line[end_pos]))
		++end_pos;

	header_value = line.substr(0, end_pos);
	line.erase(0, end_pos);

	while (!line.empty())
	{
		end_pos = 0;
		if (line.empty() || !this->isWsp(line[end_pos]))
			break ;

		++end_pos;
		while (this->isVchar(line[end_pos]))
			++end_pos;

		if (end_pos <= 1)
			break;

		header_value.append(line.substr(0, end_pos));
		line.erase(0, end_pos);

		// if (header_value.length() > 8192)
		// {
		// 	this->error_ = Response::REQUEST_HEADER_FIELDS_TOO_LARGE;
		// 	this->continue_ = false;
		// 	return ;
		// }
	}

	// Check OWS
	if (!line.empty() && this->isWsp(line[0]))
		line.erase(0, 1);

	if (!line.empty())
	{
		this->error_ = Response::BAD_REQUEST;
		this->continue_ = false;
	}

	// Add 'header' to 'request'
	this->req_->headers[header_name] = header_value;
}

void
RequestParser::parseEmptyLine(
	std::string & raw_request
)
{
	// empty-line	= CRLF

	// Console::log("RequestParser::parseEmptyLine() : ``");

	// Check done in 'header' parsing section
	// Remove 'empty line' from 'raw request' (buffer)
	raw_request.erase(0, 2);

	// Set 'parser' state
	if (this->req_->headers["Transfer-Encoding"].find("chunked") != std::string::npos)
	{
		this->state_ = CHUNKED_BODY;
	}
	// else if (this->req_->headers["Content-Length"].empty())
	// {
	// 	this->error_ = Response::LENGTH_REQUIRED;
	// 	this->continue_ = false;
	// }
	else if (!this->req_->headers["Content-Length"].empty())
	{
		this->state_ = BODY;
	}
	else{
		this->state_ = COMPLETE;
	}
}

void
RequestParser::parseBody(
	std::string & raw_request
)
{
	// message-body	= *OCTET

	std::string::size_type content_length = Converter::strToNumber<std::string::size_type>(
		this->req_->headers["Content-Length"]
	);

	if (content_length == 0)
	{
		this->state_ = COMPLETE;
		return ;
	}

	if (raw_request.empty())
	{
		this->continue_ = false;
		return ;
	}

	// TODO : Check 'server config' limit 'max_client_body_size'
	// if (content_length > XXX)
	// {
	// 	this->error_ = Response::PAYLOAD_TOO_LARGE;
	// 	this->continue_ = false;
	// 	return ;
	// }

	std::string::size_type body_length = this->req_->body.length();
	std::string::size_type free_space = content_length - body_length;

	// Copy maximum content available from 'buffer'
	//  that fit inside 'body' with 'content-length' header limit.
	std::string available = raw_request.substr(0, free_space);

	// Console::log("RequestParser::parseBody() : `" + available + "`");

	this->req_->body.append(available);
	raw_request.erase(0, available.length());

	if (available.length() >= free_space)
		this->state_ = COMPLETE;
}


void
RequestParser::parseChunkedBody(
	std::string & raw_request
)
{
	// chunked-body		= *chunk
	//						last-chunk
	//						trailer-part
	//						CRLF
	// chunk			= chunk-size [ chunk-ext ] CRLF
	//						chunk-data CRLF
	// chunk-size		= 1*HEXDIG
	// last-chunk		= 1*("0") [ chunk-ext ] CRLF
	// chunk-data		= 1*OCTET ; a sequence of chunk-size octets
	// chunk-ext		= *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
	// chunk-ext-name	= token
	// chunk-ext-val	= token / quoted-string
	// trailer-part		= *( header-field CRLF )

	// length := 0
	//  read chunk-size, chunk-ext (if any), and CRLF
	//  while (chunk-size > 0) {
	//     read chunk-data and CRLF
	//     append chunk-data to decoded-body
	//     length := length + chunk-size
	//     read chunk-size, chunk-ext (if any), and CRLF
	//  }
	//  read trailer field
	//  while (trailer field is not empty) {
	//     if (trailer field is allowed to be sent in a trailer) {
	//         append trailer field to existing header fields
	//     }
	//     read trailer-field
	//  }
	//  Content-Length := length
	//  Remove "chunked" from Transfer-Encoding
	//  Remove Trailer from existing header fields

	while (true)
	{
		std::string::size_type end_pos = raw_request.find("\r\n");

		if (end_pos == std::string::npos)
		{
			this->continue_ = false;
			return ;
		}

		// Copy 'chunk size' line
		std::string line = raw_request.substr(0, end_pos);

		// Console::log("RequestParser::parseChunkedBody() : `" + line + "`");

		// Check 'chunk size' line
		end_pos = 0;
		while (end_pos < line.length() && this->isHexDigit(line[end_pos]))
			++end_pos;

		if (end_pos == 0 || end_pos != line.length())
		{
			this->error_ = Response::BAD_REQUEST;
			this->continue_ = false;
			return ;
		}

		size_t chunk_size = Converter::hexToNumber<size_t>(line);

		// Check 'raw request' (buffer) contains full 'chunk data' line
		if (raw_request.length() < end_pos + 2 + chunk_size + 2)
		{
			this->continue_ = false;
			return ;
		}

		// Check 'CRLF' ending
		end_pos += 2;
		if (raw_request.find("\r\n", end_pos + chunk_size) != end_pos + chunk_size)
		{
			this->error_ = Response::BAD_REQUEST;
			this->continue_ = false;
			return ;
		}

		// Check 'server config' limit 'max_client_body_size' ?
		//  or check in 'request' interpreter ?
		// if (this->req_->body.length() + chunk_size > XXX)
		// {
		// 	this->error_ = Response::PAYLOAD_TOO_LARGE;
		// 	this->continue_ = false;
		// 	return ;
		// }

		// Append 'chunk data'
		this->req_->body.append(raw_request, end_pos, chunk_size);

		// Erase 'chunk size' line and 'chunk data' line
		raw_request.erase(0, end_pos + chunk_size + 2);

		// Check 'last chunk'
		if (chunk_size == 0)
		{
			// Add required 'header'
			this->req_->headers["Content-Length"] = Converter::numberToStr(this->req_->body.length());

			// Remove 'chunked' part from 'transfer-encoding'
			std::string::size_type pos;

			if ((pos = this->req_->headers["Transfer-Encoding"].find("chunked")) != std::string::npos)
				this->req_->headers["Transfer-Encoding"].erase(pos, 7);

			// Set 'parser' state
			this->state_ = COMPLETE;
			return ;
		}
	}
}

void
RequestParser::parseComplete()
{
	// Console::log("BODY : " + this->req_->body.substr(0, 20) + "..." + this->req_->body.substr(this->req_->body.length() > 20 ? this->req_->body.length() - 20 : 0));
	// Console::log("RequestParser::parseComplete()");
	this->requests_.push(this->req_);
	this->req_ = 0;
	this->state_ = REQUEST_LINE;
}

bool
RequestParser::isVchar(
	char c
) const
{
	return (31 <= c && c <= 127);
}

bool
RequestParser::isDigit(
	char c
) const
{
	return ('0' <= c && c <= '9');
}

bool
RequestParser::isAlpha(
	char c
) const
{
	return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'));
}

bool
RequestParser::isTchar(
	char c
) const
{
	return (
		   this->isDigit(c)
		|| this->isAlpha(c)
		|| std::string("!#$%&\'*+-.^_`|~").find(c) != std::string::npos
	);
}

bool
RequestParser::isWsp(
	char c
) const
{
	return (c == ' ' || c == '\t');
}

bool
RequestParser::isHexDigit(
	char c
) const
{
	return (this->isDigit(c) || ('a' <= c && c <= 'f') ||('A' <= c && c <= 'F'));
}