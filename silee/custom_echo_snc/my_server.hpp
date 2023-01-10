#ifndef my_server_HPP
# define my_server_HPP

# define BUFFER_SIZE 1024

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <exception>


class my_server
{
	public:
		class Socket_Create_Failed : public std::exception
		{
			public :
				const char * what() const throw();
		};
		
		class Bind_Failed : public std::exception
		{
			public :
				const char * what() const throw();
		};
		
		class Listen_Failed : public std::exception
		{
			public :
				const char * what() const throw();
		};

		class Client_Socket_Create_Failed : public std::exception
		{
			public :
				const char * what() const throw();
		};

		my_server(char **argv);
		~my_server(void);
		void	Make_Socket();
		void	Do_Bind();
		void	Do_listen();
		int		Get_sock() const;
		

		sockaddr_in client_address_;
		socklen_t	client_address_size_;
	private:
		int 		sock_;
		sockaddr_in server_address_;

};

#endif