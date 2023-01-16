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
		TinyServer(const char* port);
		~TinyServer();
		void run();

	private:
		int listen_fd_;
		struct addrinfo *listen_addr_;
		int kq_;
		struct kevent event_setting_;
		std::vector<int> clients_fd_;
		const static int listen_backlog_cnt_ = 10;
		const static int rdwr_buf_size_ = 1024;
		void Listen();
		void Bind();
		void Accept();
		void Action();
};

#endif