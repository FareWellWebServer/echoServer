#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstring>
# include <cstdlib>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>

# define    BACKLOG 5

/**
 * @brief 
 * Server class, use socket communication for echo server
 */
class Server {
    public:
        Server(int port);
        virtual ~Server(void);

        int     set(void);
        void    setResponse(void);
        int     run(void);
    private:
        int                 port_;    
        int                 server_fd_;
        sockaddr_in         address_;
        int                 client_fd_;
        std::string         request_;
        std::string         response_;
};

int printError(const std::string str);

#endif  // SERVER_HPP
