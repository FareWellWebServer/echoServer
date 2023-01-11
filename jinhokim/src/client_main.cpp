#include "../include/Client.hpp"

int main(int ac, char** av) {
    if (ac < 2)
        return (PrintError("Few argument error"));

    for (std::size_t i = 0; av[1][i] != 0; i++) {
        if (!std::isdigit(av[1][i]))
            return (PrintError("Port is not number"));
    }

	Client client(atoi(av[1]));

    if (client.Set() || client.Run()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
