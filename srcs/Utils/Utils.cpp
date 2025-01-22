# include "../includes/Utils/Utils.hpp"
# include <sys/stat.h>

/**
 * @brief Default constructor for Utils.
 * Private to prevent instantiation.
 */
WebServer::Utils::Utils() {}

/**
 * @brief Destructor for Utils.
 * Private to prevent deletion of instances.
 */
WebServer::Utils::~Utils() {}

/**
 * @brief Copy constructor for Utils.
 * Private to prevent copying of the Utils class.
 *
 * @param other The Utils object to copy from.
 */
WebServer::Utils::Utils(const Utils& other) { *this = other; }

/**
 * @brief Assignment operator for Utils.
 * Private to prevent assignment of the Utils class.
 *
 * @param other The Utils object to assign from.
 * @return Utils& A reference to the updated Utils object.
 */
WebServer::Utils& WebServer::Utils::operator=(const Utils& other)
{
	if (this != &other)
		*this = other;
	return *this; 
}

/**
 * @brief Handles signals received by the server.
 *
 * Logs the received signal and exits the application.
 *
 * @param signum The signal number to handle (e.g., SIGINT, SIGTERM).
 */
void WebServer::Utils::signalHandler(int signum)
{
	WebServer::Logger *logManager = WebServer::Logger::getInstance();

	logManager->logMsg(RED, "Interrup signal (%d) received.\n", signum);
	exit(signum);
}

/**
 * @brief Splits a string into a vector of substrings based on a specified delimiter.
 *
 * This method takes an input string and a delimiter, then splits the string into
 * substrings wherever the delimiter occurs. The substrings are returned as a vector.
 *
 * @param s The input string to split.
 * @param del The delimiter used to split the string. Defaults to a single space (" ").
 * @return std::vector<string> A vector containing the split substrings.
 */
std::vector<string> WebServer::Utils::splitString(const string& s, const string& del)
{
	std::vector<string> tokens;
	size_t start = 0;
	size_t end = s.find(del);

	while (end != string::npos) {
		tokens.push_back(s.substr(start, end - start));
		start = end + del.size();
		end = s.find(del, start);
	}
	tokens.push_back(s.substr(start, end - start)); // Add the last token

	return tokens;
}

/**
 * @brief Converts string to integer
 *
 * @param str The string to be converted.
 * @return int The integer converted
 */
int WebServer::Utils::ft_stoi(std::string str)
{
	std::stringstream ss(str);
	if (str.length() > 10)
		throw std::invalid_argument("ft_stoi Error: Input string length exceeds maximum allowed length");
	for (size_t i = 0; i < str.length(); ++i)
	{
		if(!isdigit(str[i]))
			throw std::invalid_argument("ft_stoi Error: Input contains non-digit characters");
	}
	int i;
	ss >> i;
	return (i);
}

/**
 * @brief gets PathType from string
 *
 * @param path The string to get PathType.
 * @return PathType 1 for File, 2 for Directory, 3 for others, -1 if stat failed.
 */
PathType WebServer::Utils::getPathType(const std::string &path)
{
	struct stat	file_stat;

	if (stat(path.c_str(), &file_stat) == 0)
	{
		if (file_stat.st_mode & S_IFREG)
			return (IS_FILE);
		else if (file_stat.st_mode & S_IFDIR)
			return (IS_DIRECTORY);
		else // Other types (symlink, socket, etc.)
			return (IS_OTHER);
	}
	return (STATFAIL);
}

/**
 * @brief Reads content from path
 *
 * @param path The file to be read.
 * @return std::string A string with the file contents.
 */
std::string WebServer::Utils::readFile(const std::string &path)
{
	if (path.empty())
		return ("");
	std::ifstream config_file(path.c_str());
	if (!config_file) 
		return ("");
	std::stringstream ss;
	ss << config_file.rdbuf();
	return (ss.str());
}

/**
 * @brief Checks if the absolute or relative path is readable
 *
 * @param abs_path_part The part added to the front of rel_path to form the absolute path  
 * @param rel_path Relative path
 * Example : "/www/html/path" where "/www/html/" is abs_path_part, while "path" is rel_path
 * @return  A string with the file contents.
 */
bool WebServer::Utils::checkFileIsReadable(const std::string &abs_path_part, const std::string &rel_path)
{
	std::string fullPath1 = rel_path;
	std::string fullPath2 = abs_path_part + rel_path;
	if ((Utils::getPathType(fullPath1) == IS_FILE 
		&& access(fullPath1.c_str(), R_OK) == 0) 
		|| (Utils::getPathType(fullPath2) == IS_FILE 
		&& access(fullPath2.c_str(), R_OK) == 0))
		return (true);
	return (false);
}

std::string WebServer::Utils::getConfigFilePath(int argc, char** argv)
{
	if (argc > 2)
	{
		WebServer::Logger *logManager = WebServer::Logger::getInstance();
		logManager->logMsg(RED, "Error: Wrong number of arguments.");
		logManager->logMsg(RED, "Usage: ./webserv or ./webserv [config file path]");
		throw std::invalid_argument("Invalid arguments.");
	}
	return ((argc == 1)? "configs/default.conf" : argv[1]);
}

// Helper function to initialise the vector
std::vector<std::pair<short, std::string> > initialiseStatusCodes()
{
	std::vector<std::pair<short, std::string> > codes;
	codes.push_back(std::make_pair(200, "OK"));
	codes.push_back(std::make_pair(201, "Created"));
	codes.push_back(std::make_pair(204, "No Content"));
	codes.push_back(std::make_pair(301, "Moved Permanently"));
	codes.push_back(std::make_pair(302, "Found"));
	codes.push_back(std::make_pair(304, "Not Modified"));
	codes.push_back(std::make_pair(400, "Bad Request"));
	codes.push_back(std::make_pair(401, "Unauthorized"));
	codes.push_back(std::make_pair(403, "Forbidden"));
	codes.push_back(std::make_pair(404, "Not Found"));
	codes.push_back(std::make_pair(500, "Internal Server Error"));
	codes.push_back(std::make_pair(501, "Not Implemented"));
	codes.push_back(std::make_pair(502, "Bad Gateway"));
	codes.push_back(std::make_pair(503, "Service Unavailable"));
	return (codes);
}

std::string WebServer::Utils::statusCodeString(short statusCode)
{
	static const std::vector<std::pair<short, std::string> > codes = initialiseStatusCodes();

	for (size_t i = 0; i < codes.size(); ++i)
	{
		if (codes[i].first == statusCode) {
			return (codes[i].second);
		}
	}
	return ("Undefined");
}

void WebServer::Utils::checkFinalToken(std::string parameters)
{
	size_t pos = parameters.rfind(';');
	if (pos != parameters.size() - 1)
		throw std::invalid_argument("Invalid Token: Missing ';'");
	parameters.erase(pos);
}

bool WebServer::Utils::isPrivateIP(const std::string& ip)
{
	return (ip.find("10.") == 0) ||
		   (ip.find("192.168.") == 0) ||
		   (ip.find("172.") == 0 && ip.size() > 4 && (ip[4] >= '1' && ip[4] <= '3'));
}

bool WebServer::Utils::isLoopbackIP(const std::string& ip)
{
	return (ip.substr(0, 4) == "127.");
}

bool WebServer::Utils::isValidIP(const std::string& ip)
{
	std::istringstream ss(ip);
	std::string part;
	int dotcount = 0;

	while (std::getline(ss, part, '.'))
	{
		for (size_t i = 0; i < part.size(); ++i) 
		{
			if (!std::isdigit(part[i]))
				return (false);
		}

		int num = std::atoi(part.c_str());
		if (num < 0 || num > 255)
			return (false);
		dotcount++;
	}
	return (dotcount == 3);
}

bool WebServer::Utils::isValidPort(const int &port)
{
	return (port >= 1024 && port <= 65535);
}
