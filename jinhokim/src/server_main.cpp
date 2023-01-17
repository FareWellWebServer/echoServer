#include "../include/Server.hpp"

int main(int ac, char** av) {
  try {
    CheckArgument(ac, av);

    Server server(atoi(av[1]));

    server.Set();
    server.Run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
