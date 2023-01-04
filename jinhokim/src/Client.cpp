#include "../include/Client.hpp"

Client::Client(std::string hostname, int port) {
    _hostname = hostname;
    _port = port;
}

Client::~Client(void) {
    ;
}

const std::string Client::getHostname(void) const {
	return this->_hostname;
}

int Client::getPort(void) const {
	return this->_port;
}
