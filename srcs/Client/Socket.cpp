#include "../../includes/Client/Socket.hpp"

// Default Constructor
WebServer::Socket::Socket(int domain, int service,  int protocol, int port, unsigned long interface)
{
    // Define address struct
    this->_address.sin_family = domain;
    this->_address.sin_port = htons(port);
    this->_address.sin_addr.s_addr = interface;
    memset(this->_address.sin_zero, '\0', sizeof(this->_address.sin_zero));
    // Establish Socket
    this->_server_fd = socket(domain, service, protocol);
    // Set Max Try for Socket 
    this->_max_try = 10;
}

// Default Destructor
WebServer::Socket::~Socket()
{
    close(this->getServerFd());
}

WebServer::Socket::Socket(const Socket& temp) { *this = temp; }

WebServer::Socket& WebServer::Socket::operator=(const Socket &temp)
{
    if (this != &temp) {
        this->_server_fd = temp.getServerFd();
        this->_connection = temp.getConnection();
        this->_max_try = temp.getMaxTry();
        this->_address = temp.getAddress();
    }
    return (*this);
}

//Socket Class Function
void WebServer::Socket::bindConnection()
{
    struct sockaddr_in address = this->getAddress();
    int addr_len = sizeof(address);
    this->setConnection(bind(this->getServerFd(), (struct sockaddr *)&address, addr_len));
    this->testConnection(this->getConnection());
}

int WebServer::Socket::acceptConnection()
{
    struct sockaddr_in address = this->getAddress();
    int addr_len = sizeof(address);
    int new_socket = accept(this->getServerFd(), (struct sockaddr *)&address, (socklen_t*)&addr_len);
    this->testConnection(new_socket);
    return new_socket;
}
    
void WebServer::Socket::listenConnection()
{
    this->testConnection(listen(this->getServerFd(), this->getMaxTry()));
}


void WebServer::Socket::testConnection(int test_variable)
{
    if (test_variable < 0) {
        perror("Failed to connect ...");
        exit(EXIT_FAILURE);
    }
}

// Getters
struct sockaddr_in WebServer::Socket::getAddress() const { return this->_address; }

int WebServer::Socket::getServerFd() const { return this->_server_fd; }

int WebServer::Socket::getConnection() const { return this->_connection; }

int WebServer::Socket::getMaxTry() const { return this->_max_try; }

// Setters
void WebServer::Socket::setConnection(int connection) { this->_connection = connection; }
