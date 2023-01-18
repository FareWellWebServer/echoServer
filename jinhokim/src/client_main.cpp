#include "Client.hpp"

int main(int ac, char** av) {
  try {
    CheckArgument(ac, av);

    Client client(atoi(av[1]));

    client.Set();
    client.Run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
