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
# include <vector>

# include "../HTTPMessage.hpp"
# include "../../Utils/Utils.hpp"

using std::cout;
using std::endl;
using std::string;

//MACROS 
# define SP " "

/**
 * @class HTTPRequest
 * @brief A class representing an HTTP request message.
 *
 * The HTTPRequest class is derived from the HTTPMessage class and provides additional
 * functionality specific to HTTP requests. It includes attributes and methods for handling
 * the HTTP method, request target, and HTTP version of the request message.
 */
class HTTPRequest: public HTTPMessage
{
    private:
        string	_method;  /**< The HTTP method of the request (e.g., GET, POST). */
		string	_request_target; /**< The target resource of the HTTP request (e.g., "/index.html"). */
		string	_http_version; /**< The HTTP version used in the request (e.g., "HTTP/1.1"). */

		void	_parseMessage(const string& message);
        void	_parseStartline(const string& start_line);
        void	_parseHeaders(const string& headers);
        void	_parseBody(const string& body);

        //Private Setters 
        void	_setMethod(const string& method);
        void	_setRequestTarget(const string& request_target);
        void	_setHttpVersion(const string& http_version);
    public:
        HTTPRequest();
        ~HTTPRequest();
        HTTPRequest(const HTTPRequest& src);
        HTTPRequest& operator=(const HTTPRequest& src);
        HTTPRequest(const string& message);

        //Getters
		string	getRequestMethod()	const;
		string	getRequestTarget()	const;
		string	getHttpVersion()	const;

        //Exceptions
		/**
         * Exception for cases where headers are missing in the HTTP message.
         */
        class HeadersDoNotExist: public std::exception
        {
            public:
                virtual const char* what() const throw()
                {
                    return "Headers doesn't exits in HTTP message";
                }
        };
		/**
         * Exception for cases where errors in request line in HTTP Request
         */
        class RequestLineError: public std::exception
        {
            public:
                virtual const char* what() const throw()
                {
                    return "Error in request Line in HTTP request header";
                }
        };

        // Abstract Method(s)
        /**
         * Abstract method for additional validation or checks specific to HTTP requests.
         * Must be implemented by derived classes.
         */
		void checker();
};
