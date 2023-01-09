#include "../include/Client.hpp"

Client::Client(const int &port, const std::string &address, int sin_family) {
  // set the client socket
  client_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd_ < 0) {
    throw std::runtime_error("Client error");
    return;
  }

  // set the server_adder
  server_addr_.sin_family = sin_family;
  server_addr_.sin_addr.s_addr = inet_addr(address.c_str());  // localhost
  server_addr_.sin_port = htons(port);
}

void Client::Connect(void) {
  if (connect(client_fd_, reinterpret_cast<sockaddr *>(&server_addr_),
              sizeof(server_addr_)) < 0) {
    throw std::runtime_error("Client error");
    return;
  }
}

void Client::Run(void) {
  // Read data from the user and send it to the server
  while (true) {
    std::cout << "Enter message: ";
    std::string message;
    std::getline(std::cin, message);
    if (message.empty()) {
      break;
    }
    ssize_t bytes_sent = send(client_fd_, message.c_str(), message.size(), 0);
    if (bytes_sent < 0) {
      throw std::runtime_error("Client error");
      return;
    }

    // Receive the response from the server
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_fd_, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
      throw std::runtime_error("Client error");
      return;
    }
    std::cout << "Received: " << std::string(buffer, bytes_received)
              << std::endl;
  }
}

void Client::UnConnect(void) { close(client_fd_); }

Client::~Client(void) { UnConnect(); }
