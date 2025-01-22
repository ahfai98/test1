#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <map>
#include "../Utils/Utils.hpp"

class Location
{
	private:
		std::string					_path;
		std::string					_root;
		bool						_autoindex;
		std::string					_index;
		std::vector<short>			_methods;
		std::string					_return;
		std::string					_alias;
		std::vector<std::string>	_cgi_exec_path;
		std::vector<std::string>	_cgi_ext;
		unsigned long				_client_max_body_size;
		bool						methods_flag;
		bool						autoindex_flag;
		bool						maxsize_flag;

	public:
		std::map<std::string, std::string> _ext_path;
		Location();
		Location(const Location &other);
		Location &operator=(const Location &src);
		~Location();

		//setter
		void setPath(std::string path);
		void setRoot(std::string root);
		void setAutoindex(std::string flag);
		void setIndex(std::string index);
		void setMethods(std::vector<std::string> methods);
		void setReturn(std::string ret);
		void setAlias(std::string alias);
		void setCgiPath(std::vector<std::string> path);
		void setCgiExtension(std::vector<std::string> ext);
		void setMethodsFlag(bool flag);
		void setAutoindexFlag(bool flag);
		void setMaxSizeFlag(bool flag);
		void setClientMaxBodySize(std::string size);
		void setClientMaxBodySize(unsigned long size);
		
		//getter
		const std::string &getPath() const;
		const std::string &getRoot() const;
		const bool &getAutoindex() const;
		const std::string &getIndex() const;
		const std::vector<short> &getMethods() const;
		const std::string &getReturn() const;
		const std::string &getAlias() const;
		const std::vector<std::string> &getCgiPath() const;
		const std::vector<std::string> &getCgiExtension() const;
		const unsigned long &getMaxBodySize() const;
		const bool &getMethodsFlag() const;
		const bool &getAutoIndexFlag() const;
		const bool &getMaxSizeFlag() const;

		//debug print
		void printLocationDetails() const;
};

#endif
