#include "../include/Server.hpp"

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

Server::Server(const int& port) : socket_option_(1) {
  // Create the server socket
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    throw std::runtime_error("server error: socket create faild");
  }
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_,
                 sizeof(socket_option_)) == -1) {
    throw std::runtime_error(strerror(errno));
  }
  server_fds_.push_back(server_fd);

  // Bind the socket to a local address and port
  sockaddr_in server_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  server_addrs_.push_back(server_addr);

  // TODO: 추후 kQueuHandler로 위임.
  // init kqueue;
  kq_ = kqueue();
  if (kq_ == -1) {
    throw std::runtime_error("server error: kq error");
  }
  // init kevent
  EV_SET(&event_, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
  if (kevent(kq_, &event_, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("server error : kq event failed");
  }
  if (event_.flags & EV_ERROR) {
    throw std::runtime_error("server error : event someting wrong");
  }
}

Server::~Server(void) { close(kq_); }

void Server::Run() {
  Bind();
  Listen();
  while (true) {
    Action();
  }
}

void Server::AddPort(const int port) {
  // TODO: server 생성자와 중복으로 추후 분리 필요
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    throw std::runtime_error("server error: socket create faild");
  }
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_,
                 sizeof(socket_option_)) == -1) {
    throw std::runtime_error(strerror(errno));
  }
  server_fds_.push_back(server_fd);

  sockaddr_in server_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  server_addrs_.push_back(server_addr);

  // init kevent
  EV_SET(&event_, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
  if (kevent(kq_, &event_, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("server error : kq event failed");
  }
  if (event_.flags & EV_ERROR) {
    throw std::runtime_error("server error : event someting wrong");
  }
  //
}

void Server::Accept(const int server_fd) {
  sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr),
                         &client_addr_len);
  if (client_fd < 0) {
    throw std::runtime_error("server error: accept failed");
  }
  EV_SET(&event_, client_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
  if (kevent(kq_, &event_, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("server error: kevnet failed");
  }
  clients_.push_back(client_fd);
  std::cout << "new connection"
            << " client fd:" << client_fd << std::endl;
}

int Server::Recv(int& client_fd, char* buffer, int flags) {
  ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, flags);
  if (bytes_received == 0) {
    close(client_fd);
    // Remove client from kqueue
    EV_SET(&event_, client_fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
    if (kevent(kq_, &event_, 1, NULL, 0, NULL) == -1) {
      throw std::runtime_error("server error: kevent failed");
    }
    // Remove client from clients vector
    for (int i = 0; i < clients_.size(); i++) {
      if (clients_[i] == client_fd) {
        clients_.erase(clients_.begin() + i);
        break;
      }
    }
    if (bytes_received == -1) {
      throw std::runtime_error("server error: recv failed");
    }
  }
  return bytes_received;
}

void Server::Send(int& client_fd, char* buffer, int flags, int bytes_received) {
  ssize_t bytes_sent = send(client_fd, buffer, bytes_received, flags);
  if (bytes_sent < 0) {
    throw std::runtime_error("server error: send failed");
    return;
  }
}

void Server::Bind(void) {
  for (int i = 0; i < server_addrs_.size(); i++) {
    if (bind(server_fds_[i], reinterpret_cast<sockaddr*>(&(server_addrs_[i])),
             sizeof(server_addrs_[i])) < 0) {
      throw std::runtime_error("server error: bind failed");
    }
  }
}

void Server::Listen(void) {
  // Start listening for incoming connections
  for (int i = 0; i < server_fds_.size(); i++) {
    if (listen(server_fds_[i], BACKLOG) < 0) {
      throw std::runtime_error("server error: listen failed");
    }
  }
}

void Server::Action() {
  struct kevent event_trigger[BACKLOG];
  char buffer[BUFFER_SIZE];
  int exist_events = kevent(kq_, NULL, 0, event_trigger, BACKLOG, NULL);
  if (exist_events == -1) {
    throw std::runtime_error("server error: kevent failed");
  }
  for (int i = 0; i < exist_events; i++) {
    if (i < server_fds_.size() && event_trigger[i].ident == server_fds_[i]) {
      Accept(server_fds_[i]);
      continue;
    }
    // already connection
    int client_fd = event_trigger[i].ident;
    int bytes_received = Recv(client_fd, buffer, 0);
    Send(client_fd, buffer, 0, bytes_received);
  }
}
