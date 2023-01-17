#ifndef TINYSERVER_HPP
#define TINYSERVER_HPP

#include <sys/event.h>
#include <sys/socket.h> // socket()
#include <netdb.h> //struct addrinfo
#include <vector>
#include <exception>
#include <iostream>
#include <string>
#include <sstream>


class TinyServer {
	public:
		TinyServer(char** port, int portCount);
		~TinyServer();
		void run();

	private:
		std::vector<int> listen_fd_;
		std::vector<struct addrinfo *> listen_addr_;
		int kq_;
		struct kevent event_setting_;
		std::vector<int> clients_fd_;
		const static int listen_backlog_cnt_ = 10;
		const static int rdwr_buf_size_ = 1024;
		void Listen();
		void Bind(int listen_fd, const struct sockaddr *ai_addr, socklen_t ai_addrlen);
		void Accept(int listen_fd);
		void Action();
};

#endif