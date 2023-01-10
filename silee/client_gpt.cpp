#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const int BUFFER_SIZE = 1024;

int main(int argc, char* argv[])
{
    // Check the number of arguments
    if (argc != 3) {
        std::cerr << "Usage: echo_client <server_ip> <port>" << std::endl;
        return 1;
    }

    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create a socket" << std::endl;
        return 1;
    }

    // Connect to the server
    sockaddr_in server_address;

    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(std::atoi(argv[2]));
    
    if (connect(sock, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        std::cerr << "Failed to connect to the server" << std::endl;
        return 1;
    }

    std::cout << "Connected to " << argv[1] << ":" << argv[2] << std::endl;

    // Send messages to the server and receive them back
    while (true) {
        std::string message;
        std::cout << "Enter a message to send to the server (or 'exit' to quit): ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        int bytes_sent = send(sock, message.c_str(), message.length(), 0);
        if (bytes_sent < 0) {
            std::cerr << "Failed to send message to server" << std::endl;
            break;
        }

        char buffer[BUFFER_SIZE];
        int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            std::cerr << "Failed to receive message from server" << std::endl;
            break;
        }

        std::cout << "Received message from server: " << buffer << std::endl;
    }

    // Close the socket
    close(sock);

    return 0;
}
