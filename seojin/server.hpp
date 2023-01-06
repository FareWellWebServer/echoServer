#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h> /* execve, dup, dup2, pipe */
#include <sys/socket.h> /* AF_INET, SOCK_STREAM, gai_strerror, socket, accept, listen, send, recv, bind, connect, getaddrinfo, freeaddrinfo, setsockopt, getsockname */
#include <cstring> /* memset, strerror */
#include <sys/errno.h> /* errno */
#include <arpa/inet.h> /* htons, htonl, ntohs, ntohl */
#include <sys/select.h> /* select */
#include <poll.h> /* poll */
#include <sys/types.h> /* kqueue kevent */
#include <netdb.h> /* getprotobyname */
#include <fcntl.h> /* fcntl */
#include <iostream>

#define MAXLINE 1000000
#define MAXBUF 1000000


class Server
{

private:
	std::string host_, port_;
	int listenfd_, clientfd_, status_;
	pollfd fds_[129];



public:
	void OpenListeningSocket(const std::string& host, const std::string& port);
	void InitPollfd(int max_connect);
	void Run( void );
	void Echo( int connfd );

	Server();
	Server( const std::string& host, const std::string& port );
	~Server();

	void Listen( void );


};


#endif