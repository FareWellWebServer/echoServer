#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <unistd.h>
# include <map>
# include <vector>
# include <netdb.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <fcntl.h>

# define    BACKLOG 1024

/**
 * @brief 
 * Server class, use socket communication for echo server
 */
class Server {
    public:
        Server(int port);
        virtual ~Server(void);

		std::string	getIp(void);
        void    	changeEvents(std::vector<struct kevent>& change_list, int socket, int16_t filter,
                                uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        void    	disconnect_client(int client_fd, std::map<int, std::string>& clients);

        int     set(void);
        void    setResponse(void);
        int     run(void);
    private:
        int                 port_;    
        int                 server_fd_;
        sockaddr_in         address_;
        std::string         request_;
        std::string         response_;
};

int printError(const std::string str);

#endif  // SERVER_HPP
