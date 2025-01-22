#ifndef LOGGER_HPP
#define LOGGER_HPP

# include <pthread.h>
# include <iostream>
# include <cstdarg>
# include <stdio.h>

using std::cout;
using std::endl;
using std::cerr;
using std::string;

// MACROS
// ANSI escape codes for colored output
# define RESET			"\x1B[0m" /**< Resets text formatting to default. */
# define RED				"\x1B[31m" /**< Red text color. */
# define LIGHT_RED		"\x1B[91m" /**< Light red text color. */
# define WHITE			"\x1B[37m" /**< White text color. */
# define BLINK			"\x1b[5m" /**< Blinking text. */
# define YELLOW			"\x1B[33m" /**< Yellow text color. */
# define LIGHT_BLUE		"\x1B[94m" /**< Light blue text color. */
# define CYAN			"\x1B[36m" /**< Cyan text color. */
# define DARK_GREY		"\x1B[90m" /**< Dark grey text color. */
# define LIGHTMAGENTA	"\x1B[95m" /**< Light magenta text color. */
// TIME OFFSET
# define GST				8; /**< Malaysia time offset for GMT+8. */

/**
 * @namespace WebServer
 * @brief Contains all components related to the web server.
 */
namespace WebServer
{
	    /**
     * @class Logger
     * @brief A thread-safe Singleton class for logging messages with colored output and timestamps.
     *
     * The Logger class provides functionality for logging messages to the console with support
     * for colored text using ANSI escape codes. It also ensures thread safety through the use of
     * a `pthread_mutex_t` and follows the Singleton design pattern to ensure only one instance exists.
     *
     * Key features include:
     * - Logging messages with customizable color formatting.
     * - Retrieving the current timestamp in a formatted string.
     * - Thread safety through a mutex.
     * - Singleton instance management.
     */
	class Logger
	{
		private:
			static Logger *instancePtr; /**< Pointer to the Singleton instance of Logger. */
			static pthread_mutex_t mtx; /**< Mutex for thread-safe Singleton access. */

			Logger();
			~Logger();
			Logger(const Logger &other);
			Logger &operator=(const Logger &other);
		public:
			static Logger* getInstance();

			void	logMsg(const char *color, const char *msg, ...);
			string	getCurrTime();
	};
} // namespace WebServer

#endif
