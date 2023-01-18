#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <sys/event.h>
#include <sys/socket.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "Response.hpp"

class Server {
 public:
  Server(int port);
  virtual ~Server(void);

  void ChangeEvents(int socket, int16_t filter, uint16_t flags, uint32_t fflags,
                    intptr_t data, void* udata);
  void DisconnectClient(int client_fd);

  int Set(void);
  int Run(void);

  int GetServerIdx(int sock);
  int IsServer(uintptr_t ident);
  int HandleErrorEvent(uintptr_t ident);
  int HandleReadEvent(uintptr_t ident);

 private:
  int port_;
  std::vector<int> server_fds_;
  std::vector<sockaddr_in> address_;
  std::map<int, std::string> clients_;
  std::vector<struct kevent> change_list_;
};

void CheckArgument(int ac, char** av);
int PrintError(const std::string str);

#endif  // SERVER_HPP
