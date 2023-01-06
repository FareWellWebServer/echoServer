#include "../include/Server.hpp"

int main(int ac, char **av) {
    if (checkArgument(ac, av)) {
        return EXIT_FAILURE;
    }

	Server server(atoi(av[1]));

    if (server.set() || server.run()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
