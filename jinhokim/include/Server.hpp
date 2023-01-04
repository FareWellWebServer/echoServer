#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <iostream>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>

# define    PORT 8080
# define    BACKLOG 5

class Server {
    public:
        Server(void);
        virtual ~Server(void);

        int createSocket(void);
        int bindSocket(void);

        int set(void);
        int run(void);
    private:
        int                 _server_fd;
        sockaddr_in         _address;
        sockaddr_in         _client_address;
        int                 _client_fd;
        std::string         _request;
        std::string         _response;
};

int printError(const std::string str);

#endif  // SERVER_HPP_