#include "../include/Server.hpp"

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

Server::Server(const int& port) : socket_option_(1) {
  // Create the server socket
  server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd_ < 0) {
    throw std::runtime_error("server error: socket create faild");
  }
  if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &socket_option_,
                 sizeof(socket_option_)) == -1) {
    throw std::runtime_error(strerror(errno));
  }

  // Bind the socket to a local address and port
  server_addr_.sin_family = AF_INET;
  server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr_.sin_port = htons(port);
  // init kevent
  struct kevent ev_set;
  EV_SET(&ev_set, server_fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
}

Server::~Server(void) { close(server_fd_); }

void Server::Run() {
  Bind();
  Listen();
  Action();
}

int Server::Accept(void) {
  sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr),
                         &client_addr_len);
  if (client_fd < 0) {
    throw std::runtime_error("server error: accept failed");
    return 0;
  }
  return client_fd;
}

void Server::Bind(void) {
  if (bind(server_fd_, reinterpret_cast<sockaddr*>(&server_addr_),
           sizeof(server_addr_)) < 0) {
    throw std::runtime_error("server error: bind failed");
  }
}

void Server::Listen(void) {
  // Start listening for incoming connections
  if (listen(server_fd_, BACKLOG) < 0) {
    throw std::runtime_error("server error: listen failed");
  }
}

void Server::Action() {
  // Accept incoming connections one by one and handle them in a separate thread
  while (true) {
    const int client_fd = Accept();
    std::cout << "New client connected" << std::endl;

    // Read data from the client and send it back
    char buffer[BUFFER_SIZE];
    while (true) {
      ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
      if (bytes_received < 0) {
        throw std::runtime_error("server error: recv failed");
        return;
      }
      if (bytes_received == 0) {
        // Client disconnected
        break;
      }
      ssize_t bytes_sent = send(client_fd, buffer, bytes_received, 0);
      if (bytes_sent < 0) {
        throw std::runtime_error("server error: send failed");
        return;
      }
    }
    std::cout << "Client disconnected" << std::endl;
    close(client_fd);
  }
}
