#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <cstring>
# include <cstdlib>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>

/**
 * @brief 
 * Client class, use socket communication for echo server
 */
class Client
{
    public:
        Client(std::string hostname, int port);
        virtual ~Client(void);

        int createSocket(void);
        int setServer(void);
        int connectServer(void);
        int sendRequest(std::string message) const;

        int set(void);
        int run(void);
    private:
        const std::string   _hostname;
        const int           _port;
        int                 _client_fd;
        hostent*            _server;
        sockaddr_in         _server_address;
        std::string         _response;
};

int printError(const std::string str);
int checkArgument(int ac, char **av);

#endif  // CLIENT_HPP
