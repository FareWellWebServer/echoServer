#ifndef CLIENT_HPP_
# define CLIENT_HPP_

# include <iostream>
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
        /**
         * @brief Construct a new Client object
         * 
         * @param port 
         */
        Client(int port);
        /**
         * @brief Destroy the Client object
         * Default destrutor
         */
        virtual ~Client(void);

        int getPort(void) const;
    private:
        int _port;
};

#endif  // CLIENT_HPP_