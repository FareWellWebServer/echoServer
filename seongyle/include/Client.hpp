#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

/**
 * @brief 
 * clinet class, use socket communication for echo server
 */
class Client
{
	public:
	/**
	 * @brief Construct a new Client object
	 * 
	 * @param fd 
	 */
		explicit Client(const int fd);
	/**
	 * @brief Destroy the Client object
	 * 
	 */
		virtual ~Client(void);
	private:

};

#endif