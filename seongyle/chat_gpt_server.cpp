#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int PORT = 42;
const int BACKLOG = 10;
const int BUFFER_SIZE = 1024;

int main()
{
    // Create the server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // Bind the socket to a local address and port
    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // Start listening for incoming connections
    if (listen(server_fd, BACKLOG) < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // Accept incoming connections one by one and handle them in a separate thread
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
        if (client_fd < 0) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "New client connected" << std::endl;

        // Read data from the client and send it back
        char buffer[BUFFER_SIZE];
        while (true) {
            ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_received < 0) {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                return EXIT_FAILURE;
            }
            if (bytes_received == 0) {
                // Client disconnected
                break;
            }
            ssize_t bytes_sent = send(client_fd, buffer, bytes_received, 0);
            if (bytes_sent < 0) {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                return EXIT_FAILURE;
            }
        }
        std::cout << "Client disconnected" << std::endl;
        close(client_fd);
    }

    // Close the server socket
    close(server_fd);
    return EXIT_SUCCESS;
}
