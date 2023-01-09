#include "../include/Server.hpp"

int main(int ac, char **av) {
    if (ac < 2)
        return (printError("Few argument error"));

    for (std::size_t i = 0; av[1][i] != 0; i++) {
        if (!std::isdigit(av[1][i]))
            return (printError("Port is not number"));
    }

	Server server(atoi(av[1]));

    if (server.set() || server.run())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
