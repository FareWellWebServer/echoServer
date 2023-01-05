#ifndef ECHOSERVER_HPP
#define ECHOSERVER_HPP

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

#define MAXLINE 100000
#define MAXBUF 100000


struct rio_t
{
	int 	fd;
	int		cnt;
	char*	bufptr;
	char	buf[MAXBUF];
};



#endif