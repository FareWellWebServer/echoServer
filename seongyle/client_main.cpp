#include "../include/Client.hpp"

int	main_process(int port) {	
	Client client(port);
}

int main(int ac, char** av) {
	try {
		if (ac < 3) {
		std::cerr << "please, input one argument(port)." << std::endl;
		return EXIT_FAILURE;
		}
		main_process(atoi(av[1]));
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}