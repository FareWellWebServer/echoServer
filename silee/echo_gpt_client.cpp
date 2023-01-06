#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int kBufferSize = 1024;

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <server_ip> <port>" << std::endl;
    return 1;
  }

  // Create a socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Set up the server address
  sockaddr_in serv_addr;
  std::memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(std::stoi(argv[2]));
  if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid server IP address" << std::endl;
    return 1;
  }

  // Connect to the server
  sockaddr* addr_ptr = reinterpret_cast<sockaddr*>(&serv_addr);
  if (connect(sockfd, addr_ptr, sizeof(serv_addr)) < 0) {
    std::cerr << "Error connecting to server" << std::endl;
    return 1;
  }

  std::cout << "Connected to server at " << argv[1] << ":" << argv[2] << std::endl;

  char buffer[kBufferSize];
  while (true) {
    std::memset(buffer, 0, kBufferSize);
    std::cout << "Enter a message to send to the server (or 'q' to quit): ";
    std::string message;
    std::getline(std::cin, message);
    if (message == "q") {
      break;
    }

    // Send the message to the server
    ssize_t bytes_sent = write(sockfd, message.c_str(), message.size());
    if (bytes_sent < 0) {
      std::cerr << "Error sending message to server" << std::endl;
      break;
    }

    // Read the response from the server
    ssize_t bytes_received = read(sockfd, buffer, kBufferSize - 1);
    if (bytes_received < 0) {
      std::cerr << "Error receiving response from server" << std::endl;
      break;
    }

    std::cout << "Received response from server: " << buffer << std::endl;
  }

  // Close the connection
  close(sockfd);

  return 0;
}
