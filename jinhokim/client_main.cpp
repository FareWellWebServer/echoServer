#include "include/Client.hpp"

// int main_process(int port) {
//     std::cout << port << std::endl;
//     return EXIT_SUCCESS;
// }

int main(int ac, char** av) {
    try {
        if (ac < 2) {
            std::cerr << "Argument error: few argument" << std::endl;
            return EXIT_FAILURE;
        }

        std::string portStr(av[1]);

        for (char const& c: portStr) {
            if (!std::isdigit(c)) {
                std::cerr << "Argument error: port is not number" << std::endl;
                return EXIT_FAILURE;
            }
        }

        //main_process(atoi(port.c_str()));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}