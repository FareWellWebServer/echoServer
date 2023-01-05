// #include <unistd.h> /* execve, dup, dup2, pipe, read, write */
// #include <sys/socket.h> /* AF_INET, SOCK_STREAM, gai_strerror, socket, accept, listen, send, recv, bind, connect, getaddrinfo, freeaddrinfo, setsockopt, getsockname */
// #include <cstring> /* memset, strerror */
// #include <iostream>
// #include <fstream>
#include "echoServer.hpp"

void echo(int connfd);

int main(int ac, char* av[])
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char clientHostname[10000], clientPort[10000];


	if (ac != 2)
	{
		std::cerr << "usage: " << av[0] << " <port>\n";
		exit(0);
	}

	listenfd = openFd::openListenFd(av[1]);

	while (1)
	{
		// clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, reinterpret_cast<struct sockaddr *>(&clientaddr), &clientlen);
		getnameinfo(reinterpret_cast<struct sockaddr *>(&clientaddr), clientlen,\
					clientHostname, 10000,\
					clientPort, 10000,\
					NI_NUMERICHOST);
		std::cout << "Connected to (" << clientHostname << ", " << clientPort << ")\n";
		echo(connfd);
		close(connfd);
	}
	return 0;
}


void echo(int connfd)
{
	size_t n;
	char buf[10000];

	n = recv(connfd, buf, 10000, 0);
	std::cout << "Server received " << n << "bytes\n";
	send(connfd, buf, n, 0);
	return ;
}

