#include "../include/Client.hpp"

int main(int ac, char** av) {
    if (checkArgument(ac, av)) {
        return EXIT_FAILURE;
    }

	Client client(av[1], atoi(av[2]));

    if (client.set() || client.run()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
