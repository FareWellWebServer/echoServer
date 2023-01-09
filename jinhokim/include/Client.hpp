#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>

class Client
{
    public:
        Client(int port);
        virtual ~Client(void);

        int set(void);
        int run(void);
    private:
        const int           port_;
        int                 client_fd_;
        hostent*            server_;
        sockaddr_in         server_address_;
        std::string         response_;
};

int printError(const std::string str);

#endif  // CLIENT_HPP
