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

#define MAXLINE 1000000
#define MAXBUF 1000000
#define MAXLISTEN 10
#define BACKLOG 128
#define DISABLE 0
#define ENABLE 1


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
	void			Delete(const std::string& host, \
							const std::string& port, \
							int fd);
	bool			IsListenFd( int fd );
	struct Node*	NewNode(const std::string& host, \
							const std::string& port, \
							int fd, \
							int status);

public:
	void	Listen( const std::string& host, const std::string& port );
	void	Run( void );

	Server();
	~Server();


};


#endif