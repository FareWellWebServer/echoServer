#include "my_client.hpp"

const char * my_client::Socket_Create_Failed::what() const throw() {
    return ("Socket Create Failed");
}

const char * my_client::Connect_Failed::what() const throw() {
    return ("Socket Create Failed");
}

const char * my_client::NoBytesRecived::what() const throw() {
    return ("Socket Create Failed");
}

const char * my_client::NoBytesSent::what() const throw() {
    return ("Socket Create Failed");
}

my_client::~my_client(void) {
}

my_client::my_client(char **argv) {
    std::memset(&server_address_, 0, sizeof(server_address_));
    server_address_.sin_family = AF_INET;
    server_address_.sin_addr.s_addr = inet_addr(argv[1]);
    server_address_.sin_port = htons(std::atoi(argv[2]));
}

void my_client::Make_Socket() {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        std::cerr << "Failed to make socket" << std::endl;
        throw Socket_Create_Failed();
    }
}

void my_client::Do_Connect() {
    if (connect(sock_, reinterpret_cast<sockaddr*>(&server_address_),
        sizeof(server_address_)) < 0)
        throw my_client::Connect_Failed();
}

void my_client::Send_Msg() {
    std::string msg;
    std::cout << "Input : " << std::endl;
    std::getline(std::cin, msg);

    if (msg == "exit")
        exit(0);
    int bytes_sent = send(sock_, msg.c_str(), msg.length(), 0);
    if (bytes_sent < 0)
        throw my_client::NoBytesSent();
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0 ,BUFFER_SIZE - 1);
    int bytes_received = recv(sock_, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0)
        throw my_client::NoBytesRecived();
    std::cout << "Received msg From Server : " << buffer << std::endl;
}

void my_client::Close_Socket() {
    close(sock_);
}
