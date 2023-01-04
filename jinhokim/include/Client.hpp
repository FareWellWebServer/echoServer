#ifndef CLIENT_HPP_
# define CLIENT_HPP_

# include <iostream>
# include <cstring>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>

# define    PORT 8080

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

        int sendMessage(std::string message) const;
        int receiveMessage(void);

		const std::string	getHostname(void) const;
		int					getPort(void) const;
		int					getClientFd(void) const;
		const std::string   getResponse(void) const;
    private:
        const std::string   _hostname;
        const int           _port;
        int                 _clientFd;
        hostent*            _server;
        sockaddr_in         _server_address;
        std::string         _response;
};

#endif  // CLIENT_HPP_
