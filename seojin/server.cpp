// #include <unistd.h> /* execve, dup, dup2, pipe, read, write */
// #include <sys/socket.h> /* AF_INET, SOCK_STREAM, gai_strerror, socket, accept, listen, send, recv, bind, connect, getaddrinfo, freeaddrinfo, setsockopt, getsockname */
// #include <cstring> /* memset, strerror */
// #include <iostream>
// #include <fstream>
#include "server.hpp"

#define DISABLE 0
#define ENABLE 1
#define MAXCONNECTION 128

Server::Server() : host_("127.0.0.1"), port_("4242") {}
Server::Server(const std::string& host, const std::string& port) : host_(host), port_(port) {}

Server::~Server()
{
	close(listenfd_);
}

/* privave functions */
void Server::OpenListeningSocket(const std::string& host, const std::string& port)
{
	struct addrinfo hints, *listp, *p;
	int optval = ENABLE;
	memset(&hints, 0, sizeof(struct addrinfo));

	/*
	SOCK_STREAM
	Use ai_family, ai_socktype, ai_protocol for socket()
	*/
	hints.ai_socktype = SOCK_STREAM;

	/*
	AI_PASSIVE
	Use returned sockaddr(ai_addr) for bind() and listen()
	In short, use sockaddr(ai_addr) for server side.
	*/
	hints.ai_flags = AI_PASSIVE;

	/*
	AI_ADDRCONFIG
	If the system only has IPv4 addresses configured,
	then getaddrinfo will only return IPv4 addresses.
	If the system only has IPv6 addresses configured,
	then getaddrinfo will only return IPv6 addresses.
	*/
	hints.ai_flags |= AI_ADDRCONFIG;
	
	/*
	AI_NUMERICSERV
	Consider `port' param as `number' (ex: 80, 4242...)
	not `service name' (ex: http, ftp...)
	*/
	hints.ai_flags |= AI_NUMERICSERV;


	/*
	AI_NUMERICHOST
	Consider `host' param as `number' (ex: 127.0.0.1)
	not `domain name' (ex: www.google.com)
	*/
	hints.ai_flags |= AI_NUMERICHOST;


	status_ = getaddrinfo(host.c_str(), port.c_str(), &hints, &listp);
	if (status_ != 0)
	{
		listenfd_ = -1;
		gai_strerror(status_);
		return;
	}


	for(p = listp; p; p = p->ai_next)
	{
		listenfd_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenfd_ < 0)
			continue;

		/*
		Set socket option.
		SOL_SOCKET - It specifies that the options being
					set or retrieved are socket options
		SO_REUSERADDR - Even if port is already occupied by another
						socket, we can use this port again.
		optval - Defined ENABLE
		*/
		setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, \
					reinterpret_cast<const void*>(&optval), \
					sizeof(int));

		if (bind(listenfd_, p->ai_addr, p->ai_addrlen) == 0)
			break;
		close(listenfd_);
	}

	freeaddrinfo(listp);

	if(p == NULL)
	{
		status_ = -1;
		listenfd_ = -1;
		std::cerr << gai_strerror(status_);
		return;
	}

	if (listen(listenfd_, 1024) < 0)
	{
		close(listenfd_);
		status_ = -1;
		return;
	}
	std::cout << host << " is listening port on " << port << "\n";
}

void Server::InitPollfd(int max_connect)
{
	fds_[0].fd = listenfd_;
	fds_[0].events = POLLIN;
	for(int i = 1; i <= max_connect; ++i)
		fds_[i].fd = -1;
}
void Server::Run( void )
{
	char		buffer[MAXBUF], client_host[MAXBUF], client_port[MAXBUF];
	int			num_ready, connfd;
	socklen_t	client_len;
	struct		sockaddr_storage clientaddr;


	while (1)
	{
		num_ready = poll(fds_, MAXCONNECTION + 1, -1);
		if (fds_[0].revents & POLLIN)
		{
			client_len = sizeof(struct sockaddr_storage);
			connfd = accept(listenfd_, reinterpret_cast<struct sockaddr *>(&clientaddr), &client_len);
			getnameinfo(reinterpret_cast<struct sockaddr *>(&clientaddr), client_len,\
						client_host, MAXBUF,\
						client_port, MAXBUF,\
						0);
			for(int i = 1; i <= MAXCONNECTION + 1; ++i)
			{
				if (fds_[i].fd < 0)
				{
					fds_[i].fd = connfd;
					fds_[i].events = POLLIN;
					break;
				}
			}
			std::cout << "Connected to (" << client_host << ", " << client_port << ")\n";
		}
		else
		{
			for(int i = 1; i <= MAXCONNECTION + 1; ++i)
			{
				if (fds_[i].revents & POLLIN)
				{
					Echo(fds_[i].fd);
					break;
				}
			}
		}
	}
}
void Server::Echo( int connfd )
{
	ssize_t n, receiveCnt;
	char c, buf[MAXBUF];

	while (1)
	{
		for(n = 0; n < MAXLINE; ++n)
		{
			receiveCnt = recv(connfd, &c, 1, 0);
			if (receiveCnt == 1)
			{
				buf[n] = c;
				if (c == '\n')
				{
					++n;
					break;
				}
			}
			else if (receiveCnt == 0)
			{
				if (n == 0)
					return;
				else
					break;
			}
			else
				return;
		}
		std::cout << "Server received " << n << " bytes\n";
		buf[n] = '\0';
		send(connfd, buf, n, 0);
		return ;
	}
}


/* public functions */
void Server::Listen( void )
{
	OpenListeningSocket(host_, port_);
	InitPollfd( MAXCONNECTION );
	Run();
}

int main(int ac, char* av[])
{
	std::string port("8080");
	std::string host("10.18.241.128");
	Server server(host, port);
	server.Listen();

	return 0;
}



// void echo(int connfd);

// int main(int ac, char* av[])
// {
// 	int listenfd, connfd;
// 	socklen_t clientlen;
// 	struct sockaddr_storage clientaddr;
// 	char clientHostname[MAXLINE], clientPort[MAXLINE];

// 	if (ac != 2)
// 	{
// 		std::cerr << "usage: " << av[0] << " <port>\n";
// 		exit(0);
// 	}

// 	listenfd = ft::OpenListenFd(av[1]);

// 	while (1)
// 	{
// 		clientlen = sizeof(struct sockaddr_storage);
// 		connfd = accept(listenfd, reinterpret_cast<struct sockaddr *>(&clientaddr), &clientlen);
// 		getnameinfo(reinterpret_cast<struct sockaddr *>(&clientaddr), clientlen,\
// 					clientHostname, MAXLINE,\
// 					clientPort, MAXLINE,\
// 					0);
// 		std::cout << "Connected to (" << clientHostname << ", " << clientPort << ")\n";
// 		echo(connfd);
// 		close(connfd);
// 	}
// 	return 0;
// }




