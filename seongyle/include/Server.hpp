#ifndef SERVER_HPP
#define SERVER_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <vector>

enum event {
  SIZE = 100,
};

class Server {
 public:
  explicit Server(const int& port);
  virtual ~Server(void);
  void Run();
  void AddPort(const int port);

 private:
  const static int BACKLOG = 10;
  const static int BUFFER_SIZE = 1024;
  int socket_option_;
  std::vector<int> server_fds_;
  std::vector<sockaddr_in> server_addrs_;
  struct kevent event_;
  int kq_;
  std::vector<int> clients_;
  /// @brief Accept the client, and return client fd
  /// @param  void
  /// @return client fd
  void Accept(const int idx);
  int Recv(int& client_fd, char* buffer, int flags);
  void Send(int& client_fd, char* buffer, int flags, int bytes_received);
  void Bind(void);
  void Listen(void);
  void Action();
};

#endif
