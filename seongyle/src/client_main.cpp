#include "../include/Client.hpp"

void	main_process(int port) {	
	Client client(port, "127.0.0.1");
	client.ftConnect();
	client.run();
	client.ftUnConnect();
}

int main(int ac, char** av) {
	try {
		if (ac != 2)
			throw std::runtime_error("please, input one argument(port).");
		main_process(atoi(av[1]));
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}