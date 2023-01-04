#ifndef CLIENT_HPP_
# define CLIENT_HPP_

# include <iostream>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>

# define    PORT 8080

/**
 * @brief 
 * Client class, use socket communication for echo server
 */
class Client
{
    public:
        /**
         * @brief Construct a new Client object
         * 
         * @param hostname 
         * @param port 
         */
        Client(std::string hostname, int port);
        /**
         * @brief Destroy the Client object
         * Default destrutor
         */
        virtual ~Client(void);
    private:
        std::string _hostname;
        int         _port;
};

#endif  // CLIENT_HPP_
