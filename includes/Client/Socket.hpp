#pragma once

# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h> // for functions like socket, bind, listen, accept and others
# include <netinet/in.h> // some macros for socket
# include <netdb.h> // getaddrinfo
# include <string.h> // strerror
# include <vector>
# include <fcntl.h>
# include <iostream>
# include <unistd.h>
# include <csignal>


using std::cout;
using std::endl;
using std::string;

namespace WebServer
{
    class Socket
    {
        private:
            int _server_fd;
            int _connection;
            int _max_try;
            struct sockaddr_in _address;
        public:
            Socket(int domain, int service, int protocol, int port, unsigned long interface);
            ~Socket();
            Socket(const Socket& temp);
            Socket &operator=(const Socket &temp);

            // Getters
            struct sockaddr_in getAddress() const;
            int getServerFd() const;
            int getConnection() const;
            int getMaxTry() const;

            // Setters
            void setConnection(int connection);

            // Socket Class Functions
            void bindConnection();
            void listenConnection();
            int acceptConnection();
            void testConnection(int test_variable);
    };
}
