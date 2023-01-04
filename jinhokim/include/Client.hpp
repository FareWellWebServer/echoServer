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

		const std::string	getHostname(void) const;
		int					getPort(void) const;
		int					getClientFd(void) const;
        int                 getServerIp(void) const;
    private:
        const std::string   _hostname;
        const int           _port;
        int                 _clientFd;
        hostent*            _server;
        sockaddr_in         _server_address;
};

#endif  // CLIENT_HPP_
