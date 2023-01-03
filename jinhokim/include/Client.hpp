#ifndef CLIENT_HPP_
# define CLIENT_HPP_

# include <iostream>

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
         * @param fd 
         */
        Client(int fd);
        /**
         * @brief Destroy the Client object
         * Default destrutor
         */
        virtual ~Client(void);

        int getFd(void) const;
    private:
        int _fd;
};

#endif  // CLIENT_HPP_