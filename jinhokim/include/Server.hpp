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

# include "Response.hpp"

# define BACKLOG 1024


class Server {
    public:
        Server(int port);
        virtual ~Server(void);

		std::string	GetIp(void);
        void    	ChangeEvents(std::vector<struct kevent>& change_list, int socket, int16_t filter,
                                uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        void    	DisconnectClient(int client_fd, std::map<int, std::string>& clients);

        int     Set(void);
        int     Run(void);
    private:
        int                 port_;    
        int                 server_fd_;
        sockaddr_in         address_;
};

void    CheckArgument(int ac, char** av);
int     PrintError(const std::string str);

#endif  // SERVER_HPP
