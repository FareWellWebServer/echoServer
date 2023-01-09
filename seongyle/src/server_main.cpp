#include "../include/Server.hpp"

void main_process(const int& port) {
  Server server(port);
  server.Run();
}

int main(int ac, char** av) {
  try {
    if (ac != 2) throw std::runtime_error("please, input one argument(port).");
    main_process(atoi(av[1]));
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}