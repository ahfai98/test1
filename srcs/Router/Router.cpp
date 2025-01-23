#include "../../includes/Router/Router.hpp"
#include "../../includes/Logger/Logger.hpp"
#include "../includes/HTTPMessage/HTTPRequest/HTTPRequest.hpp"

Router::Router(){}

Router::~Router(){}

void Router::setupServers(std::vector<Server> servers)
{
	WebServer::Logger *logManager = WebServer::Logger::getInstance();
	logManager->logMsg(CYAN, "Initializing Servers...");
	_servers = servers;
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		std::vector<std::pair<std::string, uint16_t> > tmp = _servers[i].getHostPortPairs();
		for (size_t j = 0; j < tmp.size(); ++j)
		{
            std::pair<std::string, uint16_t> pair = tmp[j];
            int used_fd = 0;

            // Check if the host-port pair is already used
			for (std::map<std::pair<std::string, uint16_t>, int>::iterator it = pairs_to_fds_map.begin();
				it != pairs_to_fds_map.end(); ++it)
			{
                if (it->first.first == pair.first && it->first.second == pair.second)
				{
                    used_fd = it->second;
                    break;
                }
            }
			if (used_fd == 0)
			{
				int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
				if (listen_fd  == -1)
				{
					logManager->logMsg(RED, "webserv: socket error %s   Closing ....", strerror(errno));
					exit(EXIT_FAILURE);
				}
				int option_value = 1;
				setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value));
				_servers[i].setServerAddress(pair.first, pair.second);
				if (bind(listen_fd, (struct sockaddr *) &_servers[i].getServerAddress(), sizeof(_servers[i].getServerAddress())) == -1)
				{
					logManager->logMsg(RED, "webserv: bind error %s   Closing ....", strerror(errno));
					exit(EXIT_FAILURE);
				}
                _servers[i].addListenFds(listen_fd);
                fds_to_servers_map[listen_fd].push_back(_servers[i]);
				pairs_to_fds_map[pair] = listen_fd;
            }
			else
			{
                // Reuse the existing socket
                _servers[i].addListenFds(used_fd);
				fds_to_servers_map[used_fd].push_back(_servers[i]);
            }
		}
	}
}

/**
 * Runs main loop that goes through all file descriptors from 0 till the biggest fd in the set.
 * - check file descriptors returend from select():
 *      if server fd --> accept new client
 *      if client fd in read_set --> read message from client
 *      if client fd in write_set:
 *          1- If it's a CGI response and Body still not sent to CGI child process --> Send request body to CGI child process.
 *          2- If it's a CGI response and Body was sent to CGI child process --> Read outupt from CGI child process.
 *          3- If it's a normal response --> Send response to client.
 * - servers and clients sockets will be added to _recv_set_pool initially,
 *   after that, when a request is fully parsed, socket will be moved to _write_set_pool
 */
void	Router::runServers()
{
	fd_set	recv_set_cpy;
	fd_set	write_set_cpy;
	int		select_ret;

	_biggest_fd = 0;
	initialiseSets();
	struct timeval timer;
	while (true)
	{
		timer.tv_sec = 1;
		timer.tv_usec = 0;
		recv_set_cpy = _recv_fd_pool;
		write_set_cpy = _write_fd_pool;
		WebServer::Logger *logManager = WebServer::Logger::getInstance();
		//select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
		//nfds: Highest-numbered fd + 1 for monitoring
		//exceptfds: Fd set to monitor for errors
		// timeout: Maximum time select() should block, if NULL, select() blocks indefinitely
		// Returns >0 for the number of fds read of I/O
		// Returns 0 if timeout occurred(no fd is ready)
		// Returns <0 if error occurred(signal interrupt, invalid parameters)
		// Interrupted System Call(EINTR), Invalid parameters(EBADF, EINVAL), Resource Issues(ENOMEM)
		if ((select_ret = select(_biggest_fd + 1, &recv_set_cpy, &write_set_cpy, NULL, &timer)) < 0)
		{
			logManager->logMsg(RED, "webserv: select error %s   Closing ....", strerror(errno));
			exit(1);
		}
		//FD_ISSET(int d, fd_set *set)
		// Returns non-zero if the fd is part of the set and ready for I/O
		// returns 0 if the fd is not part of the set or not ready for I/O
		for (int i = 0; i <= _biggest_fd; ++i)
		{
			//accept client connection if fd from server side and receive set
			if (FD_ISSET(i, &recv_set_cpy) && fds_to_servers_map.count(i))
			{
				acceptNewConnection(i);
			}
		}
	}
}

/**
 * Accept new incomming connection.
 * Create new Client object and add it to _client_map
 * Add client socket to _recv_fd_pool
*/
void	Router::acceptNewConnection(int listen_fd)
{
	struct sockaddr_in	client_address;
	long				client_address_size = sizeof(client_address);
	int					client_socket;
	char				buf[INET_ADDRSTRLEN];

	//accept() accepts new incoming connection, will block until connection request is received
	//client_address contains client's address information(IP and Port)
	//Returns new fd used for communication with the client
	//Returns -1 if  fail
	WebServer::Logger *logManager = WebServer::Logger::getInstance();
	if ((client_socket = accept(listen_fd, (struct sockaddr *)&client_address,
	(socklen_t*)&client_address_size)) == -1)
	{
		logManager->logMsg(RED, "webserv: accept error %s", strerror(errno));
		return ;
	}
	//inet_ntop converts an IP address from binary format to string
	//inet_ntop(int af, const void *src, char *dst, socklen_t size)
	// af: address family, AF_INET for IPv4, AF_INET6 for IPv6
	// src: A pointer to the buffer containing the IP address in binary format
	// dst: A pointer to the character array where the result is stored.
	// size: size of the destination buffer. Must be large enough to hold result string.
	// For IPv4 at least INET_ADDSTRLEN. Defined as 16 in <netinet/in.h>
	// Returns dst, or NULL if fail
	logManager->logMsg(LIGHT_BLUE, "New Connection From %s, Assigned Socket %d",inet_ntop(AF_INET, &client_address, buf, INET_ADDRSTRLEN), client_socket);
	addToFdSet(client_socket, _recv_fd_pool); //add client socket to recv fd pool
	/*
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0) //set to non-block mode
	{
		logManager->logMsg(RED, "webserv: fcntl error %s", strerror(errno));
		removeFromFdSet(client_socket, _recv_fd_pool);
		close(client_socket);
		return ;
	}
	*/
	logManager->logMsg(LIGHTMAGENTA, "+++++++ Connection Accepted ++++++++\n");

	char buffer[30000] = {0};
	int valread = read(client_socket, buffer, 30000);
	string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nnihao world!";
	logManager->logMsg(YELLOW, "------- Header -------\n");
	logManager->logMsg(YELLOW, "%s\n", buffer);
	HTTPRequest request(buffer);
	logManager->logMsg(LIGHT_BLUE, "------- Getters -------\n");
	logManager->logMsg(LIGHT_BLUE, "Start line: %s\n", request.getStarline().c_str());
	cout << LIGHT_BLUE << "Field line:\n" << request.getRequestMethod() << endl;
	cout << LIGHT_BLUE << "test test\n" << request.getHttpVersion() << endl;
	logManager->logMsg(LIGHT_BLUE, "Message Body: %s\n", request.getBody().c_str());
	logManager->logMsg(LIGHT_BLUE, "+++++++ Sending Message ++++++++\n");
	send(client_socket, hello.c_str(), hello.size(), 0);
	logManager->logMsg(LIGHT_BLUE, "------------------Hello message sent-------------------%d\n", valread);
	close(client_socket);
	removeFromFdSet(client_socket, _recv_fd_pool);
	client_socket = -1;
}

/* initialise recv+write fd_sets and add all server listening sockets to _recv_fd_pool. */
void	Router::initialiseSets()
{
	//FD_ZERO must be called before using fd_set to avoid undefined behaviour
	//ensure the set is empty and is ready to have fd added with FD_SET
	//FD_SET add a fd to the set
	//FD_ISSET check if a fd is in the set
	//FD_CLR remove a fd from the set.
	FD_ZERO(&_recv_fd_pool);
	FD_ZERO(&_write_fd_pool);
	WebServer::Logger *logManager = WebServer::Logger::getInstance();

	//listen() prepares a socket to accept incoming connections from clients
	//int listen(int fd, int backlog)
	//fd of the socket created with socket() and bound to an address with bind()
	//backlog is the maximum number of pending connections that can be in
	//the socket's listen queue. If backlog is full, additional incoming connections
	//are refused until space is available. Returns 0 on success, -1 if fail
	for (std::map<int, std::vector<Server> >::const_iterator it = fds_to_servers_map.begin();
	it != fds_to_servers_map.end(); ++it)
	{
		if (listen(it->first, 512) == -1)
		{
			logManager->logMsg(RED, "webserv: listen error: %s   Closing....", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (fcntl(it->first, F_SETFL, O_NONBLOCK) < 0)
		{
			logManager->logMsg(RED, "webserv: fcntl error: %s   Closing....", strerror(errno));
			exit(EXIT_FAILURE);
		}
		addToFdSet(it->first, _recv_fd_pool);
		_biggest_fd = it->first;
	}
}

// add fd to set and update biggest fd if needed
void	Router::addToFdSet(const int i, fd_set &set)
{
	FD_SET(i, &set);
	if (i > _biggest_fd)
		_biggest_fd = i;
}

//remove fd from set and update biggest fd if needed
void	Router::removeFromFdSet(const int i, fd_set &set)
{
	FD_CLR(i, &set);
	if (i == _biggest_fd)
		_biggest_fd--;
}
