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
# include <sys/stat.h>
# include <fcntl.h>

# define BACKLOG 1024
# define BUFSIZE 1024
# define HEADER_FORMAT  "HTTP/1.1 %d %s\nContent-Length: %ld\nContent-Type: %s\n\n"
# define NOT_FOUND_CONTENT "<h1>404 Not Found</h1>\n"
# define SERVER_ERROR_CONTENT "<h1>500 Internal Server Error</h1>\n"

/**
 * @brief 
 * Server class, use socket communication for echo server
 */
class Server {
    public:
        Server(int port);
        virtual ~Server(void);

		std::string	GetIp(void);
        void    	ChangeEvents(std::vector<struct kevent>& change_list, int socket, int16_t filter,
                                uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        void    	DisconnectClient(int client_fd, std::map<int, std::string>& clients);

        int     Set(void);
        void    SetResponse(void);
        int     Run(void);
    private:
        int                 port_;    
        int                 server_fd_;
        sockaddr_in         address_;
        std::string         request_;
        std::string         response_;
};

int PrintError(const std::string str);

void fill_header(char *header, int status, long len, std::string type);
void handle_404(int asock);
void handle_500(int asock);
void find_mime(char *ct_type, char *uri);
void http_handler(int asock);

#endif  // SERVER_HPP
