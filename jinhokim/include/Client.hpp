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

		const std::string	getHostname(void) const;
		int					getPort(void) const;
    private:
        std::string _hostname;
        int         _port;
};

#endif  // CLIENT_HPP_
