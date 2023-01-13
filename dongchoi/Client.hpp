#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

/**
 * @brief
 * clinet class, use socket communication for echo server
 */
class Client {
	public:
		/**
		 * @brief Construct a new Client object
		 *
		 * @param port, addriess
		 */
		Client(const int& port, const std::string& address, int sin_family = AF_INET);
		void Connect(void);
		void Run(void);
		void UnConnect(void);
		/**
		 * @brief Destroy the Client object
		 *
		 */
		virtual ~Client(void);

	private:
		const static int BUFFER_SIZE = 1024;
		int client_fd_;
		sockaddr_in server_addr_;
};

#endif