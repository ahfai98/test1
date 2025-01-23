#include "../includes/ConfigParser/Location.hpp"
#include "../includes/Utils/Utils.hpp"
#include "../includes/ConfigParser/Server.hpp"

Location::Location()
{
	this->_path = "";
	this->_root = "";
	this->_autoindex = false;
	this->_index = "";
	this->_return = "";
	this->_alias = "";
	this->_client_max_body_size = MAX_CONTENT_LENGTH;
	this->_methods.reserve(3);
	this->methods_flag = false;
	this->autoindex_flag = false;
	this->maxsize_flag = false;
}

Location::Location(const Location &other)
{
	*this = other;
}

Location &Location::operator=(const Location &src)
{
	if (this != &src)
	{
		this->_path = src._path;
		this->_root = src._root;
		this->_autoindex = src._autoindex;
		this->_index = src._index;
		this->_cgi_path = src._cgi_path;
		this->_cgi_ext = src._cgi_ext;
		this->_return = src._return;
		this->_alias = src._alias;
		this->_methods = src._methods;
		this->_ext_path = src._ext_path;
		this->_client_max_body_size = src._client_max_body_size;
		this->methods_flag = src.methods_flag;
		this->maxsize_flag = src.maxsize_flag;
		this->autoindex_flag = src.autoindex_flag;
	}
	return (*this);
}

Location::~Location(){}

//setters
void Location::setPath(std::string path)
{
	this->_path = path;
}

void Location::setRoot(std::string root)
{
	if (WebServer::Utils::getPathType(root) != IS_DIRECTORY)
		throw Server::ErrorException("Invalid Root for location: " + root);
	this->_root = root;
}

void Location::setAutoindex(std::string flag)
{
	if (flag == "on" || flag == "off")
		this->_autoindex = (flag == "on");
	else
		throw Server::ErrorException("Invalid Autoindex for location: " + flag);
}

void Location::setIndex(std::string index)
{
	this->_index = index;
}

void Location::setMethods(std::vector<std::string> methods)
{
	for (size_t i = 0; i < 3; i++)
		this->_methods[i] = 0;
	const std::string allowed_methods[] = {"GET", "POST", "DELETE"};
	for (size_t i = 0; i < methods.size(); i++)
	{
		bool method_found = false;
		for (size_t j = 0; j < 3; j++)
		{
			if (methods[i] == allowed_methods[j])
			{
				this->_methods[j] = 1;
				method_found = true;
				break;
			}
		}
		if (method_found == false)
			throw Server::ErrorException("Invalid Allow Method for location: " + methods[i]);
	}
}

void Location::setReturn(std::string ret)
{
	this->_return = ret;
}

void Location::setAlias(std::string alias)
{
	this->_alias = alias;
}

void Location::setCgiPath(std::vector<std::string> path)
{
	this->_cgi_path = path;
}

void Location::setCgiExtension(std::vector<std::string> ext)
{
	this->_cgi_ext = ext;
}

void Location::setMethodsFlag(bool flag)
{
	this->methods_flag = flag;
}

void Location::setAutoindexFlag(bool flag)
{
	this->autoindex_flag = flag;
}

void Location::setMaxSizeFlag(bool flag)
{
	this->maxsize_flag = flag;
}

void Location::setClientMaxBodySize(std::string size)
{
	unsigned long body_size = 0;
	for (size_t i = 0; i < size.length(); i++)
	{
		if (!std::isdigit(size[i]))
			throw Server::ErrorException("Invalid client_max_body_size: " + size);
	}
	body_size = WebServer::Utils::ft_stoi(size);
	if (!body_size)
		throw Server::ErrorException("Invalid client_max_body_size:" + size);
	this->_client_max_body_size= body_size;
}

void Location::setClientMaxBodySize(unsigned long parameters)
{
	this->_client_max_body_size = parameters;
}

//getters
const std::string &Location::getPath() const
{
	return (this->_path);
}

const std::string &Location::getRoot() const
{
	return (this->_root);
}

const bool &Location::getAutoindex() const
{
	return (this->_autoindex);
}

const std::string &Location::getIndex() const
{
	return (this->_index);
}

const std::vector<short> &Location::getMethods() const
{
	return (this->_methods);
}

const std::string &Location::getReturn() const
{
	return (this->_return);
}

const std::string &Location::getAlias() const
{
	return (this->_alias);
}

const std::vector<std::string> &Location::getCgiPath() const
{
	return (this->_cgi_path);
}

const std::vector<std::string> &Location::getCgiExtension() const
{
	return (this->_cgi_ext);
}

const unsigned long &Location::getMaxBodySize() const
{
	return (this->_client_max_body_size);
}

const bool &Location::getMethodsFlag() const
{
	return (this->methods_flag);
}

const bool &Location::getAutoIndexFlag() const
{
	return (this->autoindex_flag);
}

const bool &Location::getMaxSizeFlag() const
{
	return (this->maxsize_flag);
}

//debug print
void Location::printLocationDetails() const
{
	std::cout << "==================" << std::endl;
	std::cout << "Location Details:" << std::endl;
	std::cout << "==================" << std::endl;
	std::cout << "Path: " << _path << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Autoindex: " << _autoindex << std::endl;
	std::cout << "Index: " << _index << std::endl;

	std::cout << "Methods: ";
	for (size_t i = 0; i < _methods.size(); ++i)
	{
		std::cout << _methods[i];
		if (i < _methods.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;

	std::cout << "Return: " << _return << std::endl;
	std::cout << "Alias: " << _alias << std::endl;

	std::cout << "CGI Exec Paths: ";
	for (size_t i = 0; i < _cgi_path.size(); ++i)
	{
		std::cout << _cgi_path[i];
		if (i < _cgi_path.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;

	std::cout << "CGI Extensions: ";
	for (size_t i = 0; i < _cgi_ext.size(); ++i)
	{
		std::cout << _cgi_ext[i];
		if (i < _cgi_ext.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;

	std::cout << "Client Max Body Size: " << _client_max_body_size << std::endl;
	std::cout << "Methods Flag: " << methods_flag << std::endl;
	std::cout << "Autoindex Flag: " << autoindex_flag << std::endl;
	std::cout << "Maxsize Flag: " << maxsize_flag << std::endl;
	std::cout << "=======================" << std::endl;
	std::cout << "End of Location Details" << std::endl;
	std::cout << "=======================" << std::endl;
}
