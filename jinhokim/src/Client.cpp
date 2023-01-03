#include "../include/Client.hpp"

Client::Client(int port) {
    _port = port;
}

Client::~Client(void) {
    ;
}

int Client::getFd(void) const {
    return this->_port;
}
