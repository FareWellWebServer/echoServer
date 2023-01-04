#include "../include/Client.hpp"

Client::Client(std::string hostname, int port) {
    _hostname = hostname;
    _port = port;
}

Client::~Client(void) {
    ;
}
