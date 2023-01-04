#include "include/Server.hpp"

int main(void) {
	Server server;

    if (server.set() || server.run())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
