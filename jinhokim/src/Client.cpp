#include "include/Client.hpp"

Client::Client(int fd) {
    std::cout << "fd constructor called" << std::endl;
}

Client::~Client() {
    std::cout << "Default destructor called" << std::endl;
}