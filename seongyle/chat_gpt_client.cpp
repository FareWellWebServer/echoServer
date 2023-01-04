#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

const int PORT = 42;
const int BUFFER_SIZE = 1024;

int main()
{
    // Create the client socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // Connect to the server
    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
    server_addr.sin_port = htons(PORT);
    if (connect(client_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // Read data from the user and send it to the server
    while (true) {
        std::cout << "Enter message: ";
        std::string message;
        std::getline(std::cin, message);
        if (message.empty()) {
            break;
        }
        ssize_t bytes_sent = send(client_fd, message.c_str(), message.size(), 0);
        if (bytes_sent < 0) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }

        // Receive the response from the server
        char buffer[BUFFER_SIZE];
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Received: " << std::string(buffer, bytes_received) << std::endl;
    }

    // Close the client socket
    close(client_fd);
    return EXIT_SUCCESS;
}
