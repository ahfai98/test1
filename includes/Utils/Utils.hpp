#pragma once

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <iostream>
# include <unistd.h>
# include <csignal>
# include <vector>
# include <stdlib.h>
# include <limits.h>
# include <sstream>
# include <iterator>
# include <fstream>
# include "../../includes/Logger/Logger.hpp"

using std::cout;
using std::endl;
using std::string;

#define MAX_CONTENT_LENGTH 50000000 //Maximum size of HTTP request content

enum PathType
{
	IS_FILE = 1,
	IS_DIRECTORY = 2,
	IS_OTHER = 3,
	STATFAIL = -1
};

/**
 * @namespace WebServer
 * @brief Contains all components related to the web server.
 */
namespace WebServer
{
       /**
     * @class Utils
     * @brief A utility class providing static helper functions for common tasks.
     *
     * The Utils class offers utility functions for tasks such as handling signals and
     * splitting strings. All methods are static, and the class cannot be instantiated.
     *
     * Key features include:
     * - Signal handling for the server.
     * - String manipulation utilities.
     */
	class Utils
    {
        private:
            Utils();
            ~Utils();
            Utils(const Utils& other);
            Utils& operator=(const Utils& other);
        public:
            static  void signalHandler(int signum);
            static  std::vector<string> splitString(const string& s, const string& del = " ");
			static int ft_stoi(std::string str);

			static std::string readFile(const std::string &path);
        	static bool checkFileIsReadable(const std::string &abs_path_part, const std::string &rel_path);
			
			static PathType getPathType(const std::string &path);

        	static std::string statusCodeString(short statusCode);
			static std::vector<std::pair<short, std::string> > initialiseStatusCodes();

        	static std::string getConfigFilePath(int argc, char** argv);
			static void	checkFinalToken(std::string parameters);
			static bool isPrivateIP(const std::string &ip);
			static bool isLoopbackIP(const std::string &ip);
			static bool isValidIP(const std::string &ip);
			static bool isValidPort(const int &port);
    };
} // namespace WebServer
