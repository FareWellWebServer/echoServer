#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>

# define    BACKLOG 5

class Server {
    public:
        Server(int port);
        virtual ~Server(void);

        int     set(void);
        void    setResponse(void);
        int     run(void);
    private:
        int                 _port;    
        int                 _server_fd;
        sockaddr_in         _address;
        sockaddr_in         _client_address;
        int                 _client_fd;
        std::string         _request;
        std::string         _response;
};

int printError(const std::string str);
int checkArgument(int ac, char **av);

#endif  // SERVER_HPP
