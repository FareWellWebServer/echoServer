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

#define MAXLINE 1000000
#define MAXBUF 1000000


namespace ft
{
	int OpenListenFd(char *port);
	int OpenClientFd(char *hostname, char *port);
}



#endif