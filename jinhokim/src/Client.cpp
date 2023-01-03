#include "../include/Client.hpp"

Client::Client(int fd) {
    _fd = fd;
}

Client::~Client(void) {
    ;
}

int Client::getFd(void) const {
    return this->_fd;
}
