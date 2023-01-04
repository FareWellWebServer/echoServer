#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

const int PORT = 8080;
const int BACKLOG = 5;

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    int bind_result = bind(server_fd, (sockaddr*)&address, sizeof(address));
    if (bind_result < 0)
    {
        std::cerr << "Failed to bind socket to address" << std::endl;
        return 1;
    }

    int listen_result = listen(server_fd, BACKLOG);
    if (listen_result < 0)
    {
        std::cerr << "Failed to listen on socket" << std::endl;
        return 1;
    }

    while (true)
    {
        sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_fd = accept(server_fd, (sockaddr*)&client_address, &client_len);
        if (client_fd < 0)
        {
            std::cerr << "Failed to accept incoming connection" << std::endl;
            continue;
        }

        std::cout << "Accepted connection from "
                    << inet_ntoa(client_address.sin_addr)
                    << ":"
                    << ntohs(client_address.sin_port)
                    << std::endl;

        while (true)
        {
            char buffer[1024];
            ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_received < 0)
            {
                std::cerr << "Failed to receive data from client" << std::endl;
                break;
            }
            else if (bytes_received == 0)
            {
                std::cout << "Client disconnected" << std::endl;
                break;
            }

            ssize_t bytes_sent = send(client_fd, buffer, bytes_received, 0);
            if (bytes_sent < 0)
            {
                std::cerr << "Failed to send data to client" << std::endl;
                break;
            }
        }

        close(client_fd);
    }

    return 0;
}
