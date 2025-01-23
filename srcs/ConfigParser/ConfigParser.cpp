#include "../includes/ConfigParser/ConfigParser.hpp"
#include "../includes/ConfigParser/Location.hpp"

ConfigParser::ConfigParser(): _server_num(0){}

ConfigParser::~ConfigParser(){}

/* printing parameters of servers from config file */
void ConfigParser::print()
{
	std::cout << "------------- Config -------------" << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
		_servers[i].printServerDetails();
}

//read config file, extract and parse server blocks
void ConfigParser::extractServerBlocks(const std::string &config_file)
{
	std::string		content;

	if (WebServer::Utils::getPathType(config_file) != IS_FILE)
		throw ErrorException("File is invalid");
	if (access(config_file.c_str(), R_OK) == -1)
		throw ErrorException("File is not accessible");
	content = WebServer::Utils::readFile(config_file);
	removeComments(content);
	normaliseSpaces(content);
	splitServerBlocks(content);
	for (size_t i = 0; i < this->_server_num; i++)
	{
		Server server;
		parseServerBlock(this->_server_blocks[i], server);
		this->_servers.push_back(server);
	}
}

void ConfigParser::removeComments(std::string &content)
{
	size_t comment_start = content.find('#');
	while (comment_start != std::string::npos)
	{
		size_t comment_end = content.find('\n', comment_start);
		if (comment_end == std::string::npos) 
		{
			// If no newline found, erase everything from start
			content.erase(comment_start);
			break;
		}
		else 
			// Erase from start to newline
			content.erase(comment_start, comment_end - comment_start);
		comment_start = content.find('#');
	}
}

void ConfigParser::normaliseSpaces(std::string &content)
{
	size_t i = 0;
	while (i < content.length() && isspace(content[i]))
		i++;
	content = content.substr(i);

	if (!content.empty()) 
	{
		size_t end = content.length() - 1;
		while (end > 0 && isspace(content[end]))
			end--;
		content = content.substr(0, end + 1);
	}

	std::string result;
	result.reserve(content.length());

	bool isPrevSpace = false;

	for (size_t i = 0; i < content.length(); ++i)
	{
		if (isspace(content[i]))
		{
			if (!isPrevSpace)
			{
				result += ' ';
				isPrevSpace = true;
			}
		}
		else
		{
			result += content[i];
			isPrevSpace = false;
		}
	}
	content = result;
}

//split server blocks into a vector
void ConfigParser::splitServerBlocks(std::string &content)
{
	size_t start = 0;
	size_t end = 1;

	if (content.find("server", 0) == std::string::npos)
		throw ErrorException("No server block found");

	while (start < content.length())
	{
		start = getServerBlockStart(start, content);
		end = getServerBlockEnd(start, content);
		if (start >= end)
			throw ErrorException("Invalid Scope for Server Block");
		this->_server_blocks.push_back(content.substr(start, end - start + 1));
		this->_server_num++;
		start = end + 1;
	}
}

// returns the index of the "{" at the start of a server block
size_t ConfigParser::getServerBlockStart(size_t start, std::string &content)
{
	size_t i = start;
	while (i < content.length() && isspace(content[i]))
		i++;
	if (i >= content.length() || content.compare(i, 6, "server") != 0)
		throw ErrorException("Invalid characters or nothing found for start of server block");
	i += 6;
	while (i < content.length() && isspace(content[i]))
		i++;
	if (i >= content.length() || content[i] != '{')
		throw ErrorException("Missing '{' for start of server block");
	return (i);
}

// returns the index of } at the end of server block */
size_t ConfigParser::getServerBlockEnd(size_t start, std::string &content)
{
	size_t i = start + 1;
	int depth = 0;
	while (i < content.length())
	{
		if (content[i] == '{')
			depth++;
		else if (content[i] == '}')
		{
			if (depth == 0)
				return (i);
			depth--;
		}
		i++;
	}
	throw ErrorException("Dangling '{' found in server block");
}

std::vector<std::string> splitStrToVect(const std::string &line, const std::string &sep_chars)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = 0;

	while (start < line.length())
	{
		end = line.find_first_of(sep_chars, start);
		if (end == std::string::npos)
		{
			if (start < line.length())
				result.push_back(line.substr(start));
			break;
		}
		if (end > start)
			result.push_back(line.substr(start, end - start));
		start = line.find_first_not_of(sep_chars, end);
	}
	return (result);
}

//parse ServerBlocks
void ConfigParser::parseServerBlock(std::string &config, Server &server)
{
	std::vector<std::string>	parameters;
	std::vector<std::string>	error_codes;

	parameters = splitStrToVect(config, std::string(" \n\t"));
	if (parameters.size() < 3)
		throw  ErrorException("Failed server validation");
	handlers["root"] = &ConfigParser::handleRoot;
	handlers["listen"] = &ConfigParser::handleListen;
	handlers["location"] = &ConfigParser::handleLocation;
	handlers["autoindex"] = &ConfigParser::handleAutoIndex;
	handlers["index"] = &ConfigParser::handleIndex;
	handlers["error_page"] = &ConfigParser::handleErrorPage;
	handlers["client_max_body_size"] = &ConfigParser::handleClientMaxBodySize;
	handlers["server_name"] = &ConfigParser::handleServerName;
	
	for (size_t i = 0; i < parameters.size(); i++)
	{
		std::map<std::string, Handler>::iterator it = handlers.find(parameters[i]);
		if (it != handlers.end())
		{
			if (i == parameters.size() - 1)
				throw ErrorException("Missing value for " + parameters[i]);
			(this->*(it->second))(i, server, parameters);
		}
		else if (parameters[i] != "}" && parameters[i] != "{")
		{
			if (server.getLocationSetFlag() == true)
				throw  ErrorException("parameters after location");
			else
				throw  ErrorException("Unsupported directive");
		}
	}
	finaliseServer(server);
}

void ConfigParser::finaliseServer(Server &server)
{
	if (server.getRoot().empty())
		server.setRoot("/");
	if (server.getIndex().empty())
		server.setIndex("index.html");
	if (!WebServer::Utils::checkFileIsReadable(server.getRoot(), server.getIndex()))
		throw ErrorException("Index from config file not found or unreadable");
	if (server.checkLocationsDup())
		throw ErrorException("Duplicate locations in server configuration");
	if (!server.checkErrorPages())
		throw ErrorException("Invalid error pages in server configuration");
	if (server.getLocationSetFlag() == true)
		server.setLocationsDefaultValues();
	checkServersDup();
	std::vector<Location> location_list = server.getLocations();
	std::vector<std::string> return_list;
	for (size_t i = 0; i < location_list.size() ; ++i)
	{

	}
}

//Ensure server port, host and name is unique
void ConfigParser::checkServersDup()
{
	std::map<std::pair<std::string, uint16_t>, std::string> hostPortMap;
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		Server& server = _servers[i];
		for (size_t j = 0; j < server.getHostPortPairs().size(); ++j)
		{
			std::pair<std::string, uint16_t> hostPort = server.getHostPortPairs()[j];

			// Check if host:port pair already exists in the map
			std::map<std::pair<std::string, uint16_t>, std::string>::iterator it = hostPortMap.find(hostPort);

			if (it != hostPortMap.end())
			{
				if (it->second == server.getServerName())
					throw ErrorException("Server Config Error: Duplicate Host, Port and Server Name found");
			}
			else
				hostPortMap[hostPort] = server.getServerName();
		}
	}
}

std::vector<Server>	ConfigParser::getServers()
{
	return (this->_servers);
}

void ConfigParser::handleListen(size_t &i, Server &server, std::vector<std::string> &parameters)
{
	if (server.getLocationSetFlag() == true)
		throw  ErrorException("parameters after location");
	std::string ip = "127.0.0.1";
	uint16_t port = 8000;
	i++;
	WebServer::Utils::checkFinalToken(parameters[i]);
	size_t colon_pos = parameters[i].find(":");
	if (colon_pos != std::string::npos)
	{
		ip = parameters[i].substr(0, colon_pos);
		if (ip == "localhost")
			ip = "127.0.0.1";
		std::string port_str = parameters[i].substr(colon_pos + 1);
		// Parse port
		std::istringstream port_stream(port_str);
		if (!(port_stream >> port) || !port_stream.eof())
			throw std::invalid_argument("Invalid port number.");
		if (!WebServer::Utils::isValidPort(port))
			throw std::out_of_range("Port number must be between 1024 and 65535.");
		// Parse IP
		if (!(WebServer::Utils::isValidIP(ip) && (WebServer::Utils::isPrivateIP(ip) || WebServer::Utils::isLoopbackIP(ip))))
			throw std::invalid_argument("IP address must be valid, within the private range, or a loopback address." + ip);
	}
	//case for no port
	else if ((WebServer::Utils::isValidIP(parameters[i]) && (WebServer::Utils::isPrivateIP(parameters[i]) || WebServer::Utils::isLoopbackIP(parameters[i]))))
	{
		ip = parameters[i];
		port = 80;
	}
	//case for no host IP
	else if (WebServer::Utils::isValidPort(WebServer::Utils::ft_stoi(parameters[i])))
	{
		ip = "127.0.0.1";
		port = WebServer::Utils::ft_stoi(parameters[i]);
	}
	server.addHostPort(ip, port);
}

void ConfigParser::handleLocation(size_t &i, Server &server, std::vector<std::string> &parameters)
{
	if (server.getLocationSetFlag() == false)
	{
		server.setLocationFlag(true);
		server.setServerDefaultValues();
	}
	std::string	path;
	i++;
	if (parameters[i] == "{" || parameters[i] == "}")
		throw  ErrorException("Wrong character in server scope{}");
	path = parameters[i];
	std::vector<std::string> codes;
	if (parameters[++i] != "{")
		throw  ErrorException("Wrong character in server scope{}");
	i++;
	while (i < parameters.size() &&parameters[i] != "}")
		codes.push_back(parameters[i++]);
	server.parseLocationBlocks(path, codes);
	// Check last character is }
	if (i < parameters.size() &&parameters[i] != "}")
		throw  ErrorException("Wrong character in server scope{}");
}

void ConfigParser::handleRoot(size_t &i, Server &server, std::vector<std::string> &parameters)
{
	if (server.getLocationSetFlag() == true)
		throw  ErrorException("parameters after location");
	if (!server.getRoot().empty())
		throw  ErrorException("Root is duplicated");
	server.setRoot(parameters[++i]);
}


void ConfigParser::handleIndex(size_t &i, Server &server, std::vector<std::string> &parameters)
{
	if (server.getLocationSetFlag() == true)
		throw  ErrorException("parameters after location");
	if (!server.getIndex().empty())
		throw  ErrorException("Index is duplicated");
	server.setIndex(parameters[++i]);
}

void ConfigParser::handleAutoIndex(size_t &i, Server &server, std::vector<std::string> &parameters)
{
	if (server.getLocationSetFlag() == true)
		throw  ErrorException("parameters after location");
	if (server.getAutoIndexFlag())
		throw ErrorException("Autoindex of server is duplicated");
	server.setAutoindex(parameters[++i]);
	server.setAutoindexFlag(true);
}

void ConfigParser::handleErrorPage(size_t &i, Server &server, std::vector<std::string> &parameters)
{
	if (server.getLocationSetFlag() == true)
		throw  ErrorException("parameters after location");
	std::vector<std::string>	error_codes;
	while (++i < parameters.size())
	{
		error_codes.push_back(parameters[i]);
		if (parameters[i].find(';') != std::string::npos)
			break ;
		if (i + 1 >= parameters.size())
			throw ErrorException("Wrong character out of server scope{}");
	}
	server.setErrorPages(error_codes);
}

void ConfigParser::handleServerName(size_t &i, Server &server, std::vector<std::string> &parameters)
{
	i++;
	WebServer::Utils::checkFinalToken(parameters[i]);
	if (server.getLocationSetFlag() == true)
		throw  ErrorException("parameters after location");
	if (!server.getServerName().empty())
		throw  ErrorException("Server_name is duplicated");
	server.setServerName(parameters[i]);
}

void ConfigParser::handleClientMaxBodySize(size_t &i, Server &server, std::vector<std::string> &parameters)
{
	if (server.getLocationSetFlag() == true)
		throw  ErrorException("parameters after location");
	if (server.getMaxSizeFlag())
		throw  ErrorException("Client_max_body_size is duplicated");
	server.setClientMaxBodySize(parameters[++i]);
	server.setMaxSizeFlag(true);
}

