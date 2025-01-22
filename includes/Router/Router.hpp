#ifndef ROUTER_HPP
# define ROUTER_HPP

#include "../Utils/Utils.hpp"
#include "../ConfigParser/Server.hpp"

 // setup and run servers using config file
 // establis new connections with clients and receive/send requests/responses.
class Router
{
	public:
		Router();
		~Router();
		void	setupServers(std::vector<Server>);
		void	runServers();
		
	private:
		std::vector<Server> _servers;
		std::map<int, Server> _servers_map;
		//std::map<int, Client> _clients_map;
		fd_set	_recv_fd_pool;
		fd_set	_write_fd_pool;
		int		_biggest_fd;

		void acceptNewConnection(Server &);
		//void checkTimeout();
		void initialiseSets();
		/*
		void readRequest(const int &, Client &);
		void handleReqBody(Client &);
		void sendResponse(const int &, Client &);
		void sendCgiBody(Client &, CgiHandler &);
		void readCgiResponse(Client &, CgiHandler &);
		void closeConnection(const int);
		void assignServer(Client &);
		*/
		void addToSet(const int , fd_set &);
		void removeFromSet(const int , fd_set &);
};

#endif
