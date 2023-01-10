#ifndef SERVER_HPP
#define SERVER_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <vector>
class Server {
 public:
  explicit Server(const int& port);
  virtual ~Server(void);
  void Run();

 private:
  const static int BACKLOG = 10;
  const static int BUFFER_SIZE = 1024;
  int server_fd_;
  sockaddr_in server_addr_;
  int kq_;
  struct kevent ev_set_;
  struct kevent ev_list_;
  std::vector<int> clients_;

  /// @brief Accept the client, and return client fd
  /// @param  void
  /// @return client fd
  int Accept(void);
  void Bind(void);
  void Listen(void);
  void Action();
};

#endif
