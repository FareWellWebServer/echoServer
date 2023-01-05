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
	char clientHostname[MAXLINE], clientPort[MAXLINE];


	if (ac != 2)
	{
		std::cerr << "usage: " << av[0] << " <port>\n";
		exit(0);
	}

	listenfd = openFd::openListenFd(av[1]);

	while (1)
	{
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, reinterpret_cast<struct sockaddr *>(&clientaddr), &clientlen);
		getnameinfo(reinterpret_cast<struct sockaddr *>(&clientaddr), clientlen,\
					clientHostname, MAXLINE,\
					clientPort, MAXLINE,\
					0);
		std::cout << "Connected to (" << clientHostname << ", " << clientPort << ")\n";
		echo(connfd);
		close(connfd);
	}
	return 0;
}


void echo(int connfd)
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
	}
	return ;
}

