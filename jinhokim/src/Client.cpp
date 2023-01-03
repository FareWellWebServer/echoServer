#include "../include/Client.hpp"

Client::Client(int fd) {
    std::cout << "fd constructor called" << std::endl;
    _fd = fd;
}

Client::~Client(void) {
    std::cout << "default destructor called" << std::endl;
}

int Client::getFd(void) const {
    return this->_fd;
}
