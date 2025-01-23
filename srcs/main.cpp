# include "../includes/Client/Socket.hpp"
# include "../includes/HTTPMessage/HTTPRequest/HTTPRequest.hpp"
# include "../includes/Logger/Logger.hpp"
# include "../includes/ConfigParser/ConfigParser.hpp"
# include "../includes/Router/Router.hpp"

void handleSigpipe(int sig)
{ 
	if(sig){}
}

int main(int argc, char **argv)
{
	try 
	{
        signal(SIGINT, WebServer::Utils::signalHandler);
		signal(SIGPIPE, handleSigpipe);
		std::string configFilePath = WebServer::Utils::getConfigFilePath(argc, argv);
		ConfigParser	configParser;
		configParser.extractServerBlocks(configFilePath);
		Router 	Router;
		Router.setupServers(configParser.getServers());
		Router.runServers();
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
