#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

class Client {
 public:
  Client(int port);
  virtual ~Client(void);

  int Set(void);
  int Run(void);

 private:
  const int port_;
  int client_fd_;
  sockaddr_in server_address_;
  std::string response_;
};

void CheckArgument(int ac, char** av);
int PrintError(const std::string str);

#endif  // CLIENT_HPP
