#include "TinyServer.hpp"

int main(int ac, char **av)
{
	try 
	{
		TinyServer server(av[1]);
		server.run();
		std::cout << "complete" << std::endl;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}