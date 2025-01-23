#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <arpa/inet.h>
#include <map>
#include <vector>
#include "../Utils/Utils.hpp"

class Location;

class Server
{
	private:
		std::vector< std::pair<std::string, uint16_t> > _host_port_pairs;
		std::string						_server_name; //domain name
		std::string						_root;
		std::string						_index;
		bool							_autoindex;
		unsigned long					_client_max_body_size;
		std::map<short, std::string>	_error_pages_map; //map status codes to custom error pages
		std::vector<Location> 			_locations;
		struct sockaddr_in 				_server_address;
		bool							location_flag;
		bool							autoindex_flag;
		bool							maxsize_flag;
		std::vector<int>				_listen_fds;

		typedef void (Server::*Handler)(size_t&, Location&, std::vector<std::string>&);

		std::map<std::string, Handler>	handlers;

		void handleRoot(size_t &i, Location& new_location, std::vector<std::string> &parameters);
		void handleAllowMethods(size_t &i, Location& new_location, std::vector<std::string> &parameters);
		void handleAutoIndex(size_t &i, Location& new_location, std::vector<std::string> &parameters);
		void handleIndex(size_t &i, Location& new_location, std::vector<std::string> &parameters);
		void handleAlias(size_t &i, Location& new_location, std::vector<std::string> &parameters);
		void handleReturn(size_t &i, Location& new_location, std::vector<std::string> &parameters);
		void handleCgiExt(size_t &i, Location& new_location, std::vector<std::string> &parameters);
		void handleCgiPath(size_t &i, Location& new_location, std::vector<std::string> &parameters);
		void handleClientMaxBodySize(size_t &i, Location& new_location, std::vector<std::string> &parameters);

	public:
		Server();
		~Server();
		Server(const Server &other);
		Server &operator=(const Server &src);

		//setter
		void setServerName(std::string server_name);
		void setRoot(std::string root);
		void setIndex(std::string index);
		void setAutoindex(std::string flag);
		void setClientMaxBodySize(std::string size);
		void initialiseErrorPagesMap();
		void setErrorPages(const std::vector<std::string> &parameters);
		void parseLocationBlocks(std::string path, std::vector<std::string> parameters);
		void addListenFds(int fd);
		void setAutoindexFlag(bool flag);
		void setLocationFlag(bool flag);
		void setMaxSizeFlag(bool flag);
		void setServerDefaultValues();
		void setLocationsDefaultValues();
		void setServerAddress(std::string host, uint16_t port);

		//getter
		const std::string 					&getServerName() const;
		const std::string 					&getRoot() const;
		const std::string 					&getIndex() const;
		const bool 							&getAutoindex() const;
		const size_t						&getClientMaxBodySize() const;
		const std::map<short, std::string>	&getErrorPages() const;
		const std::vector<Location>			&getLocations() const;
		const std::vector<int>				&getListenFds() const;
		const bool							&getLocationSetFlag() const;
		const bool							&getAutoIndexFlag() const;
		const bool							&getMaxSizeFlag() const;
		const struct sockaddr_in 			&getServerAddress() const;
		const std::vector< std::pair<std::string, uint16_t> >	&getHostPortPairs() const;
		
		//getter for Response
		const std::string 						&getErrorPagePath(short key);
		const std::vector<Location>::iterator	getLocation(std::string key);

		//checker functions
		bool		checkHost(const std::string &host) const;
		bool		checkErrorPages();
		void		checkLocation(Location &location) const;
		bool		checkLocationPath(const std::string &path) const;
		bool		checkLocationsDup() const;

		//helper functions for host:port map
		void		addHostPort(std::string host, uint16_t port);
		void		printHostPortPairs() const;

		//debug print
		void		printServerDetails() const;

		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "Server Config Error: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif
