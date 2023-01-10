#include "./my_server.hpp"

const char * my_server::Socket_Create_Failed::what() const throw() {
    return ("Socket Create Failed");
}

const char * my_server::Bind_Failed::what() const throw() {
    return ("Bind Failed");
}

const char * my_server::Listen_Failed::what() const throw() {
    return ("Listen Failed");
}

const char * my_server::Client_Socket_Create_Failed::what() const throw() {
    return ("Client Socket Create Failed");
}

const char * my_server::NoBytesRecived::what() const throw() {
    return ("No bytes recived");
}

const char * my_server::NoBytesSent::what() const throw() {
    return ("No bytes Sent");
}

my_server::my_server(char **argv) {
    std::memset(&server_address_, 0, sizeof(server_address_));
    server_address_.sin_family = AF_INET;
    server_address_.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address_.sin_port = htons(std::atoi(argv[1]));
    // std::memset(&client_address_, 0, sizeof(client_address_));
    socklen_t client_address_size_ = sizeof(client_address_);
}

my_server::~my_server(void) {
}

void main_server(char** argv) {
}

void my_server::Make_Socket() {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0)
        throw Socket_Create_Failed();
}

void my_server::Do_Bind() {
    if (bind(sock_, reinterpret_cast<sockaddr*>(&server_address_),
        sizeof(server_address_)) < 0)
        throw Bind_Failed();
}

void my_server::Do_listen() {
    if (listen(sock_, 10) < 0)
        throw Listen_Failed();
}

int my_server::Get_sock() const {
    return (this->sock_);
}

void my_server::Recv_n_Send(int c_sock, char *buffer) {
    std::memset(buffer, 0, BUFFER_SIZE);
    int byte_recive = recv(c_sock, buffer, BUFFER_SIZE - 1, 0);
    if (byte_recive == 0)
        throw my_server::NoBytesRecived();
    int byte_sent = send(c_sock, buffer, byte_recive, 0);
    if (byte_sent == 0)
        throw my_server::NoBytesSent();
}