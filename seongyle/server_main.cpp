#include "../include/Server.hpp"

int	main_process() {	
	Server server();
}

int main(int ac, char** av) {
	try {
		if (ac < 3) {
		std::cerr << "please, input one argument(port)." << std::endl;
		return EXIT_FAILURE;
		}
		main_process();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}