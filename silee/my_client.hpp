#ifndef my_client_HPP
# define my_client_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class my_client
{
  private:
  public:
    my_client(void);
    ~my_client(void);
    void  main_client(char **argv);
};

#endif