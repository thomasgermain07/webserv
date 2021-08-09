/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/10 15:24:28 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/13 02:45:20 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_PARSER_HPP
# define HTTP_REQUEST_PARSER_HPP

# include <string>
# include <queue>
# include "Response.hpp"
# include "Request.hpp"

class RequestParser
{
private:
	// Enum
	enum eState
	{
		REQUEST_LINE,
		HEADER,
		EMPTY_LINE,
		BODY,
		CHUNKED_BODY,
		COMPLETE
	};

public:
	typedef std::queue<Request *>		t_req_ctn;

private:
	// Attributes
	t_req_ctn				requests_;
	bool					continue_;
	Response::eStatus		error_;
	eState					state_;
	Request *				req_;

public:
	// Constructors & Destructor
	RequestParser();
	~RequestParser();

	// Method
	void parse(std::string & raw_request);

	// Getters & Setters
	bool hasRequest() const;
	Request * getRequest();
	bool hasError() const;
	Response::eStatus getError() const;

private:
	// Private methods
	void parseRequestLine(std::string & raw_request);
	void parseHeader(std::string & raw_request);
	void parseEmptyLine(std::string & raw_request);
	void parseBody(std::string & raw_request);
	void parseChunkedBody(std::string & raw_request);
	void parseComplete();

	bool isVchar(char c) const;
	bool isDigit(char c) const;
	bool isAlpha(char c) const;
	bool isTchar(char c) const;
	bool isWsp(char c) const;
	bool isHexDigit(char c) const;

private:
	// C++98 Delete
	RequestParser(const RequestParser & other);
	RequestParser & operator=(const RequestParser & other);

}; // -- class RequestParser

#endif
