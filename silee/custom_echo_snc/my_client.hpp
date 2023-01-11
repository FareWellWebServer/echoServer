#ifndef my_client_HPP
# define my_client_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

class my_client {
	public:
		class Socket_Create_Failed : public std::exception {
			public:
				const char * what() const throw();
		};
		class Connect_Failed : public std::exception {
			public:
				const char * what() const throw();
		};
		class NoBytesSent : public std::exception {
			 public:
				const char * what() const throw();
		};
		class NoBytesRecived : public std::exception {
			 public:
				const char * what() const throw();
		};
		my_client(char **argv);
		~my_client(void);
		void  main_client(char **argv);
		void Make_Socket();
		void Do_Connect();
		void Send_Msg();
		void Close_Socket();
	private:
		sockaddr_in server_address_;
		int sock_;
};

#endif
