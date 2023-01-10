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

  // TODO: 추후 kQueuHandler로 위임.
  // init kqueue;
  kq_ = kqueue();
  if (kq_ == -1) {
    throw std::runtime_error("server error: kq error");
  }
  // init kevent
  EV_SET(&event_, server_fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
  if (kevent(kq_, &event_, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("server error : kq event failed");
  }
  if (event_.flags & EV_ERROR) {
    throw std::runtime_error("server error : event someting wrong");
  }
}

Server::~Server(void) {
  close(server_fd_);
  close(kq_);
}

void Server::Run() {
  Bind();
  Listen();
  while (true) {
    Action();
  }
}

void Server::Accept(void) {
  sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr),
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
    // Removw client from clients vector
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
  struct kevent event_trigger[BACKLOG];
  char buffer[BUFFER_SIZE];
  int exist_events = kevent(kq_, NULL, 0, event_trigger, BACKLOG, NULL);
  if (exist_events == -1) {
    throw std::runtime_error("server error: kevent failed");
  }
  for (int i = 0; i < exist_events; i++) {
    if (event_trigger[i].ident == server_fd_) {
      Accept();
      continue;
    }
    // already connection
    int client_fd = event_trigger[i].ident;
    int bytes_received = Recv(client_fd, buffer, 0);
    Send(client_fd, buffer, 0, bytes_received);
  }
}
