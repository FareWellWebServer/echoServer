#include "../include/Client.hpp"

Client::Client(int port) {
    _port = port;
}

Client::~Client(void) {
    ;
}

int Client::getPort(void) const {
    return this->_port;
}
