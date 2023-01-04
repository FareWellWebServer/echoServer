#include "../include/Server.hpp"

int	main_process() {	
	Server server();
}

int main(int ac, char** av) {
	try {
		if (ac < 3) {
		throw std::runtime_error("please, input one argument(port).");
		return EXIT_FAILURE;
		}
		main_process();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}