#pragma once

# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h> // for functions like socket, bind, listen, accept and others
# include <netinet/in.h> // some macros for socket
# include <netdb.h> // getaddrinfo
# include <string.h> // strerror
# include <fcntl.h>
# include <iostream>
# include <unistd.h>
# include <csignal>
# include <map>
# include "../Utils/Utils.hpp"

using std::cout;
using std::endl;
using std::string;

//MACROS 
# define CRLF "\r\n"
# define FIELD_LINE_SEPARATOR "\r\n\r\n"

/**
 * Represents an HTTP message.
 *
 * Attributes:
 *     _start_line (string): The start line of the HTTP message (e.g., request line or status line).
 *     _headers (std::vector<KeyValue>): A collection of key-value pairs representing the HTTP headers.
 *     _body (string): The body content of the HTTP message.
 */
class HTTPMessage
{
    protected:
        string							_start_line;
        std::map<string, string>		_headers;
        string							_body;
    public:
        HTTPMessage();
        ~HTTPMessage();
        HTTPMessage(const HTTPMessage& src);
        HTTPMessage& operator=(const HTTPMessage& src);

        //Setters
        void							setHeader(const string& name, const string& value);
        void							setBody(string body);

        //Getters
        string							getFieldName(const string& name) const;
        const std::map<string, string>	getHeaders() const;
        string							getBody() const;
        string							getMessage() const;
		string							getStarline() const;

        // Abstract Method(s)
        /**
         * Abstract method for additional validation or checks.
         * Must be implemented by derived classes.
         */
        virtual void checker() = 0;
};

std::ostream&	operator<<(std::ostream& os, const std::map<string, string>& headers);
