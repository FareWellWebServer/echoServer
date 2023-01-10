#ifndef my_server_HPP
# define my_server_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class my_server
{
  private:
  public:
    my_server(void);
    ~my_server(void);
    void  main_server(char **argv);
};

#endif