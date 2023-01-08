#include "../include/Client.hpp"

int	main_process(int port) {	
	Client client(port, "127.0.0.1");
}

int main(int ac, char** av) {
	try {
		if (ac < 3) {
		throw std::runtime_error("please, input one argument(port).");
		return EXIT_FAILURE;
		}
		main_process(atoi(av[1]));
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}