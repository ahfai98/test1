#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <stdio.h>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "Server.hpp"
#include "../Utils/Utils.hpp"

class Server;

class ConfigParser
{
	private:
		std::vector<Server>			_servers;
		std::vector<std::string>	_server_blocks;
		size_t						_server_num;
		
		typedef void (ConfigParser::*Handler)(size_t&, Server&, std::vector<std::string>&);

		std::map<std::string, Handler>	handlers;
		
		void handleRoot(size_t &i, Server &server, std::vector<std::string> &parameters);
		void handleListen(size_t &i, Server &server, std::vector<std::string> &parameters);
		void handleLocation(size_t &i, Server &server, std::vector<std::string> &parameters);
		void handleIndex(size_t &i, Server &server, std::vector<std::string> &parameters);
		void handleAutoIndex(size_t &i, Server &server, std::vector<std::string> &parameters);
		void handleErrorPage(size_t &i, Server &server, std::vector<std::string> &parameters);
		void handleServerName(size_t &i, Server &server, std::vector<std::string> &parameters);
		void handleClientMaxBodySize(size_t &i, Server &server, std::vector<std::string> &parameters);

	public:
		ConfigParser();
		~ConfigParser();

		void extractServerBlocks(const std::string &config_file);
		void splitServerBlocks(std::string &content);
		void removeComments(std::string &content);
		void normaliseSpaces(std::string &content);
		size_t	getServerBlockStart(size_t start, std::string &content);
		size_t	getServerBlockEnd(size_t start, std::string &content);
		void parseServerBlock(std::string &config, Server &server);
		void checkServersDup();
		std::vector<Server> getServers();
		void print();
		void finaliseServer(Server &server);
		
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "Config Parser Error: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif
