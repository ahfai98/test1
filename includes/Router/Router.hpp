#ifndef ROUTER_HPP
# define ROUTER_HPP

#include "../Utils/Utils.hpp"
#include "../ConfigParser/Server.hpp"

 //Setup servers and route requests and responses
class Router
{
	public:
		Router();
		~Router();
		void	setupServers(std::vector<Server>);
		void	runServers();
		void printRouterDetails();
		
	private:
		std::vector<Server> _servers;
		//std::map<int, Client> _clients_map;
		fd_set	_recv_fd_pool;
		fd_set	_write_fd_pool;
		int		_biggest_fd;
		std::map<int, std::vector<Server> > fds_to_servers_map;
		std::map<std::pair<std::string, uint16_t>, int> pairs_to_fds_map;

		void acceptNewConnection(int listen_fd);
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
		void addToFdSet(const int fd, fd_set &fdset);
		void removeFromFdSet(const int fd, fd_set &fdset);
};

#endif
