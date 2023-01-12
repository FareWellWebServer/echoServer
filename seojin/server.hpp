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
#include <sys/event.h>
#include <iostream>
#include <set>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAXLINE 10000
#define MAXBUF 10000
#define MAXLISTEN 10
#define BACKLOG 128
#define DISABLE 0
#define ENABLE 1

struct rio_t
{
	int rio_fd;
	int rio_cnt;
	char *rio_bufptr;
	char rio_buf[MAXBUF];
};

#include "robust_inout.hpp"
#include "http.hpp"



struct Node
{
	std::string	host;
	std::string	port;
	int			fd;
	int			status;
};


class Server
{

private:
	int						kq_;
	struct kevent			events_[MAXLISTEN + BACKLOG];
	std::set<struct Node*>	server_;
	std::set<struct Node*>	clients_;


	void			Echo( int connfd );
	void			DeleteClientNode(const std::string& host, const std::string& port, int fd);
	void			ClearServerNode( void );
	void			ClearClientNode( void );
	bool			IsListenFd( int fd );
	struct Node*	NewNode(const std::string& host, const std::string& port, int fd, int status);

public:
	void	Listen( const std::string& host, const std::string& port );
	void	Run( void );

	Server();
	~Server();


};


#endif