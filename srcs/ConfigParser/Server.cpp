#include "../includes/ConfigParser/Server.hpp"
#include "../includes/Utils/Utils.hpp"
#include "../includes/ConfigParser/Location.hpp"
#include "../includes/Logger/Logger.hpp"

Server::Server()
{
	this->_server_name = "";
	this->_root = "";
	this->_index = "";
	this->_autoindex = false;
	this->_client_max_body_size = MAX_CONTENT_LENGTH;
	this->initialiseErrorPagesMap();
	this->location_flag = false;
	this->autoindex_flag = false;
	this->maxsize_flag = false;
}

Server::~Server(){}

Server::Server(const Server &other)
{
	*this = other;
}

Server &Server::operator=(const Server &src)
{
	if (this != &src)
	{
		this->_server_name = src._server_name;
		this->_root = src._root;
		this->_index = src._index;
		this->_autoindex = src._autoindex;
		this->_client_max_body_size = src._client_max_body_size;
		this->_error_pages_map = src._error_pages_map;
		this->_locations = src._locations;
		this->_server_address = src._server_address;
		this->location_flag = src.location_flag;
		this->autoindex_flag = src.autoindex_flag;
		this->maxsize_flag = src.maxsize_flag;
		this->_listen_fds = src._listen_fds;
	}
	return (*this);
}

void Server::setServerName(std::string server_name)
{
	this->_server_name = server_name;
}

//set root for absolute and relative path
void Server::setRoot(std::string root)
{
	WebServer::Utils::checkFinalToken(root);
	if (WebServer::Utils::getPathType(root) == IS_DIRECTORY)
	{
		this->_root = root;
		return ;
	}
	char dir[PATH_MAX];
	getcwd(dir, PATH_MAX);
	std::string full_root = dir + root;
	if (WebServer::Utils::getPathType(full_root) != IS_DIRECTORY)
		throw ErrorException("Invalid Root for server: " + root);
	this->_root = full_root;
}

void Server::setIndex(std::string index)
{
	WebServer::Utils::checkFinalToken(index);
	this->_index = index;
}

void Server::setAutoindex(std::string flag)
{
	WebServer::Utils::checkFinalToken(flag);
	if (flag != "on" && flag != "off")
		throw ErrorException("Invalid utoindex for server: " + flag);
	if (flag == "on")
		this->_autoindex = true;
}

void Server::setClientMaxBodySize(std::string size)
{
	unsigned long body_size = 0;
	WebServer::Utils::checkFinalToken(size);
	for (size_t i = 0; i < size.length(); i++)
	{
		if (!std::isdigit(size[i]))
			throw ErrorException("Invalid client_max_body_size: " + size);
	}
	body_size = WebServer::Utils::ft_stoi(size);
	if (!body_size)
		throw ErrorException("Invalid client_max_body_size: " + size);
	this->_client_max_body_size = body_size;
}

//initialise map for error pages
void Server::initialiseErrorPagesMap()
{
	_error_pages_map[301] = "";
	_error_pages_map[302] = "";
	_error_pages_map[400] = "";
	_error_pages_map[401] = "";
	_error_pages_map[403] = "";
	_error_pages_map[404] = "";
	_error_pages_map[500] = "";
	_error_pages_map[502] = "";
	_error_pages_map[503] = "";
	_error_pages_map[504] = "";
}

void Server::setErrorPages(const std::vector<std::string> &parameters)
{
	if (parameters.empty())
		return;
	if (parameters.size() % 2 != 0)
		throw ErrorException ("Error page initialization faled");
	for (size_t i = 0; i < parameters.size() - 1; i++)
	{
		//check error code is all digits
		for (size_t j = 0; j < parameters[i].size(); j++) {
			if (!std::isdigit(parameters[i][j]))
				throw ErrorException("Error code is invalid");
		}
		//check error code is 3 digits
		if (parameters[i].size() != 3)
			throw ErrorException("Error code is invalid");
		short code_error = WebServer::Utils::ft_stoi(parameters[i]);
		if (WebServer::Utils::statusCodeString(code_error)  == "Undefined" || code_error < 400)
			throw ErrorException ("Incorrect error code: " + parameters[i]);
		i++; //Move to the next string which is the path to the error page
		std::string path = parameters[i];
		WebServer::Utils::checkFinalToken(path);
		if (WebServer::Utils::getPathType(path) == IS_DIRECTORY) //If path is directory
			throw ErrorException ("Incorrect path for error page file: " + path);
		if (WebServer::Utils::getPathType(this->_root + path) != IS_FILE) //If path is not file
			throw ErrorException ("Incorrect path for error page file: " + this->_root + path);
		// If path does not exist or is not accessible
		std::string abs_path  = this->_root + path;
		if (access(abs_path.c_str(), F_OK) == -1 || access(abs_path.c_str(), R_OK) == -1)
			throw ErrorException ("Error page file :" + abs_path + " is not accessible");
		std::map<short, std::string>::iterator it = this->_error_pages_map.find(code_error);
		//If error code mapping is found, overwrite, else add new map entry
		if (it != _error_pages_map.end())
			this->_error_pages_map[code_error] = path;
		else
			this->_error_pages_map.insert(std::make_pair(code_error, path));
	}
}

void Server::parseLocationBlocks(std::string path, std::vector<std::string> parameters)
{
	Location new_location;
	std::vector<std::string> methods;

	handlers["root"] = &Server::handleRoot;
	handlers["allow_methods"] = &Server::handleAllowMethods;
	handlers["methods"] = &Server::handleAllowMethods;
	handlers["autoindex"] = &Server::handleAutoIndex;
	handlers["index"] = &Server::handleIndex;
	handlers["return"] = &Server::handleReturn;
	handlers["alias"] = &Server::handleAlias;
	handlers["cgi_ext"] = &Server::handleCgiExt;
	handlers["cgi_exec_path"] = &Server::handleCgiPath;
	handlers["client_max_body_size"] = &Server::handleClientMaxBodySize;
	
	new_location.setPath(path);
	for (size_t i = 0; i < parameters.size(); i++)
	{
		std::map<std::string, Handler>::iterator it = handlers.find(parameters[i]);
		if (it != handlers.end())
		{
			if (i == parameters.size() - 1)
				throw ErrorException("Missing value for " + parameters[i]);
			(this->*(it->second))(i, new_location, parameters);
		}
		else
			throw ErrorException("Invalid parameters for location :" + parameters[i]);
	}
	if (new_location.getPath() != "/cgi-bin" && new_location.getIndex().empty())
		new_location.setIndex(this->_index); //set index
	checkLocation(new_location);
	this->_locations.push_back(new_location);
}

void	Server::addListenFds(int fd)
{
	this->_listen_fds.push_back(fd);
}

void Server::setAutoindexFlag(bool flag)
{
	this->autoindex_flag = flag;
}

void Server::setLocationFlag(bool flag)
{
	this->location_flag = flag;
}

void Server::setMaxSizeFlag(bool flag)
{
	this->maxsize_flag = flag;
}

void Server::setServerDefaultValues()
{
	if (this->_root == "")
		this->_root = "/";
	if (this->_index == "")
		this->_index = "index.html";
	if (this->_host_port_pairs.size() == 0)
		this->_host_port_pairs.push_back(std::make_pair("127.0.0.1", 80));
}

void Server::setLocationsDefaultValues()
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i].getRoot() == "")
			_locations[i].setRoot(this->_root);
		if (_locations[i].getAutoindex() == false && _locations[i].getAutoIndexFlag() == false)
			_locations[i].setAutoindex(this->_autoindex? "on" : "off");
		if (_locations[i].getIndex() == "")
			_locations[i].setIndex(this->_index);
	}	
}

//Getter
const std::string &Server::getServerName() const
{
	return (this->_server_name);
}

const std::string &Server::getRoot() const
{
	return (this->_root);
}

const std::string &Server::getIndex() const
{
	return (this->_index);
}

const bool &Server::getAutoindex() const
{
	return (this->_autoindex);
}

const size_t &Server::getClientMaxBodySize() const
{
	return (this->_client_max_body_size);
}

const std::map<short, std::string> &Server::getErrorPages() const
{
	return (this->_error_pages_map);
}

const std::vector<Location> &Server::getLocations() const
{
	return (this->_locations);
}

const std::vector<int> &Server::getListenFds() const
{
	return (this->_listen_fds);
}

const bool &Server::getLocationSetFlag() const
{
	return (this->location_flag);
}

const bool &Server::getAutoIndexFlag() const
{
	return (this->autoindex_flag);
}

const bool &Server::getMaxSizeFlag() const
{
	return (this->maxsize_flag);
}

const std::vector< std::pair<std::string, uint16_t> > &Server::getHostPortPairs() const
{
	return (this->_host_port_pairs);
}

// helper functions for Response
const std::string &Server::getErrorPagePath(short key)
{
	std::map<short, std::string>::iterator it = this->_error_pages_map.find(key);
	if (it == this->_error_pages_map.end())
		throw ErrorException("Error_page does not exist");
	return (it->second);
}

// find Location by its path
const std::vector<Location>::iterator Server::getLocation(std::string key)
{
	std::vector<Location>::iterator it;
	for (it = this->_locations.begin(); it != this->_locations.end(); it++)
	{
		if (it->getPath() == key)
			return (it);
	}
	throw ErrorException("Error: path to location not found");
}

/*
Checks if host is valid IPv4 Address
inet_pton converts a string representing IPv4 address into binary representation
AF_INET means IPv4
Stores the binary representation into sockaddr.sin_addr
Returns 1 if conversion is successful, 0 if host is not valid IPv4 string
Returns -1 if not valid address family (AF_INET for IPv4, AF_INET6 for IPv6)
*/
bool Server::checkHost(const std::string &host) const
{
	struct sockaddr_in sockaddr;
  	return (inet_pton(AF_INET, host.c_str(), &(sockaddr.sin_addr)) == 1);
}

bool Server::checkErrorPages()
{
	std::map<short, std::string>::const_iterator it;
	for (it = this->_error_pages_map.begin(); it != this->_error_pages_map.end(); it++)
	{
		if (it->first < 100 || it->first > 599)
			return (false);
		std::string abs_path = getRoot() + it->second;
		if (access(abs_path.c_str(), F_OK) == -1 || access(abs_path.c_str(), R_OK) == -1)
			return (false);
	}
	return (true);
}

/* check parameterss of location */
void Server::checkLocation(Location &location) const
{
	if (location.getPath() == "/cgi-bin")
	{
		if (location.getCgiPath().empty() || location.getCgiExtension().empty() || location.getIndex().empty())
			throw ErrorException("Failed CGI validation");

		if (access(location.getIndex().c_str(), R_OK) == -1)
		{
			//use location's root or cwd and combine with path to get Index location
			std::string path = location.getRoot() + location.getPath() + "/" + location.getIndex();
			if (WebServer::Utils::getPathType(path) != IS_FILE)
			{				
				std::string root = getcwd(NULL, 0);
				location.setRoot(root);
				path = root + location.getPath() + "/" + location.getIndex();
			}
			if (path.empty() || WebServer::Utils::getPathType(path) != IS_FILE || access(path.c_str(), R_OK) == -1)
				throw ErrorException("Failed CGI validation");
		}
		// check if the number of Cgi paths matches with number of extensions
		if (location.getCgiPath().size() != location.getCgiExtension().size())
			throw ErrorException("Failed CGI validation");
		std::vector<std::string>::const_iterator it;
		for (it = location.getCgiPath().begin(); it != location.getCgiPath().end(); ++it)
		{
			if (WebServer::Utils::getPathType(*it) == STATFAIL)
				throw ErrorException("Failed CGI validation");
		}
		std::vector<std::string>::const_iterator it_path;
		for (it = location.getCgiExtension().begin(); it != location.getCgiExtension().end(); ++it)
		{
			std::string tmp = *it;
			if (tmp != ".py" && tmp != ".sh" && tmp != "*.py" && tmp != "*.sh")
				throw ErrorException("Failed CGI validation");
			//set _ext_path[".py"] == "/usr/bin/python"
			for (it_path = location.getCgiPath().begin(); it_path != location.getCgiPath().end(); ++it_path)
			{
				std::string tmp_path = *it_path;
				if (tmp == ".py" || tmp == "*.py")
				{
					if (tmp_path.find("python") != std::string::npos)
						location._ext_path[".py"] = tmp_path;
				}
				else if (tmp == ".sh" || tmp == "*.sh")
				{
					if (tmp_path.find("bash") != std::string::npos)
						location._ext_path[".sh"] = tmp_path;
				}
			}
		}
	}
	else
	{
		if (!checkLocationPath(location.getPath()))
			throw ErrorException("Invalid path for location");
		if (!WebServer::Utils::checkFileIsReadable(location.getRoot() + location.getPath() + "/", location.getIndex()))
			throw ErrorException("Invalid Index for location");
		if (!location.getReturn().empty() && !WebServer::Utils::checkFileIsReadable(location.getRoot(), location.getReturn()))
			throw ErrorException("Invalid Return for location");
		if (!location.getAlias().empty() && !WebServer::Utils::checkFileIsReadable(location.getRoot(), location.getAlias()))
			 	throw ErrorException("Invalid Alias for location");
	}
}

// check duplicate for location name
bool Server::checkLocationsDup() const
{
	if (this->_locations.size() < 2)
		return (false);
	for (size_t i = 0; i < this->_locations.size() - 1; ++i)
	{
		for (size_t j = i + 1; j < this->_locations.size(); ++j)
		{
			if (this->_locations[i].getPath() == this->_locations[j].getPath())
				return (true);
		}
	}
	return (false);
}

bool Server::checkLocationPath(const std::string &path) const
{
	static const std::string invalid_chars = "*?<>|\"\\\0";
	static const std::string whitespace = " \t\n\r";

	if (path.empty() || path[0] != '/')
		return (false);
	if (path.find_first_of(invalid_chars) != std::string::npos)
		return (false);
	if (path.find_first_of(whitespace) != std::string::npos)
		return (false);
	if (path.find("//") != std::string::npos)
		return (false);
	return (true);
}

//Functions for host:port map
void	Server::addHostPort(std::string host, uint16_t port)
{
	if (!checkHost(host))
		throw ErrorException("Invalid host: " + host);
	_host_port_pairs.push_back(std::make_pair(host, port));
}

void	Server::printHostPortPairs() const
{
	std::cout << "Host-Port Pairs:" << std::endl;
	for (size_t i = 0; i < _host_port_pairs.size(); ++i)
	{
		std::cout << "  Host: " << _host_port_pairs[i].first 
		<< ", Port: " << _host_port_pairs[i].second << std::endl;
	}
}

//Handlers
void Server::handleRoot(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	if (!new_location.getRoot().empty()) //check if root is already set
		throw ErrorException("Root of location is duplicated");
	WebServer::Utils::checkFinalToken(parameters[++i]);
	if (WebServer::Utils::getPathType(parameters[i]) == IS_DIRECTORY) //if directory
		new_location.setRoot(parameters[i]);
	else
		new_location.setRoot(this->_root + parameters[i]); //if not directory
}
		
void Server::handleAllowMethods(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	if (new_location.getMethodsFlag()) //check if methods already set
			throw ErrorException("Allow_methods of location is duplicated");
		std::vector<std::string> methods;
		while (++i < parameters.size())
		{
			if (parameters[i].find(";") != std::string::npos)
			{
				WebServer::Utils::checkFinalToken(parameters[i]);
				methods.push_back(parameters[i]);
				break ;
			}
			else
			{
				methods.push_back(parameters[i]);
				if (i + 1 >= parameters.size())
					throw ErrorException("Token is invalid");
			}
		}
		new_location.setMethods(methods);
		new_location.setMethodsFlag(true);
}

void Server::handleAutoIndex(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	if (new_location.getPath() == "/cgi-bin")
		throw ErrorException("parameters autoindex not allow for CGI");
	if (new_location.getAutoIndexFlag()) //check if autoindex already set
		throw ErrorException("Autoindex of location is duplicated");
	WebServer::Utils::checkFinalToken(parameters[++i]);
	new_location.setAutoindex(parameters[i]);
	new_location.setAutoindexFlag(true);
}

void Server::handleIndex(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	//check if index already set
	if (!new_location.getIndex().empty()) 
		throw ErrorException("Index of location is duplicated");
	WebServer::Utils::checkFinalToken(parameters[++i]);
	new_location.setIndex(parameters[i]);
}

void Server::handleAlias(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	if (new_location.getPath() == "/cgi-bin")
		throw ErrorException("parameters alias not allow for CGI");
	//check if alias already set
	if (!new_location.getAlias().empty())
		throw ErrorException("Alias of location is duplicated");
	WebServer::Utils::checkFinalToken(parameters[++i]);
	new_location.setAlias(parameters[i]);
}

void Server::handleReturn(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	if (new_location.getPath() == "/cgi-bin")
		throw ErrorException("parameters return not allow for CGI");
	//check if return already set
	if (!new_location.getReturn().empty())
		throw ErrorException("Return of location is duplicated");
	WebServer::Utils::checkFinalToken(parameters[++i]);
	new_location.setReturn(parameters[i]);
}

void Server::handleCgiExt(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	if (new_location.getPath() != "/cgi-bin")
		throw ErrorException("parameters cgi_ext only allowed for /cgi-bin");
	std::vector<std::string> extension;
	while (++i < parameters.size())
	{
		if (parameters[i].find(";") != std::string::npos)
		{
			WebServer::Utils::checkFinalToken(parameters[i]);
			extension.push_back(parameters[i]);
			break ;
		}
		else
		{
			extension.push_back(parameters[i]);
			if (i + 1 >= parameters.size())
				throw ErrorException("Token is invalid");
		}
	}
	new_location.setCgiExtension(extension);
}

void Server::handleCgiPath(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	if (new_location.getPath() != "/cgi-bin")
		throw ErrorException("parameters cgi_path only allowed for /cgi-bin");
	std::vector<std::string> path;
	while (++i < parameters.size())
	{
		if (parameters[i].find(";") != std::string::npos)
		{
			WebServer::Utils::checkFinalToken(parameters[i]);
			path.push_back(parameters[i]);
			break ;
		}
		else
		{
			path.push_back(parameters[i]);
			if (i + 1 >= parameters.size())
				throw ErrorException("Token is invalid");
		}
		//check if python or bash is present
		if (parameters[i].find("/python") == std::string::npos &&parameters[i].find("/bash") == std::string::npos)
			throw ErrorException("cgi_path is invalid");
	}
	new_location.setCgiPath(path);
}

void Server::handleClientMaxBodySize(size_t &i, Location& new_location, std::vector<std::string> &parameters)
{
	if (new_location.getMaxSizeFlag()) //check if max body size already set
		throw ErrorException("Maxbody_size of location is duplicated");
	WebServer::Utils::checkFinalToken(parameters[++i]);
	new_location.setClientMaxBodySize(parameters[i]);
	new_location.setMaxSizeFlag(true);
}

//debug print
void Server::printServerDetails() const
{
	std::cout << "================" << std::endl;
	std::cout << "Server Details:" << std::endl;
	std::cout << "================" << std::endl;
	std::cout << "Server Name: " << _server_name << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Index: " << _index << std::endl;
	std::cout << "Autoindex: " << _autoindex << std::endl;
	std::cout << "Client Max Body Size: " << _client_max_body_size << std::endl;
	printHostPortPairs();

	std::cout << "Error Pages Map:" << std::endl;
	for (std::map<short, std::string>::const_iterator it = _error_pages_map.begin(); it != _error_pages_map.end(); ++it)
	{
		std::cout << " Status Code: " << it->first << ", Page: " << it->second << std::endl;
	}

	// Server Address
	std::cout << "Server Address: " << inet_ntoa(_server_address.sin_addr) 
	<< ":" << ntohs(_server_address.sin_port) << std::endl;

	std::cout << "Location Flag: " << location_flag << std::endl;
	std::cout << "Autoindex Flag: " << autoindex_flag << std::endl;
	std::cout << "Maxsize Flag: " << maxsize_flag << std::endl;

	std::cout << "Listen File Descriptors:" << std::endl;
	for (size_t i = 0; i < _listen_fds.size(); ++i)
	{
		std::cout << _listen_fds[i];
		if (i < _listen_fds.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;

	std::cout << "Locations:" << std::endl;
	for (size_t i = 0; i < _locations.size(); ++i)
	{
		std::cout << "  Location " << (i + 1) << ":" << std::endl;
		_locations[i].printLocationDetails();
	}

	std::cout << "=======================" << std::endl;
	std::cout << "End of Server Details" << std::endl;
	std::cout << "=======================" << std::endl;
}