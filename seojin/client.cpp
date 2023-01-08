#include "echoServer.hpp"


int main(int ac, char* av[])
{
	int clientfd, receiveCnt;
	char *host, *port, sendBuf[MAXBUF], recvBuf[MAXBUF];

	if (ac != 3)
	{
		std::cerr << "usage: " << av[0] << " <host> <port>\n";
		exit(0);
	}

	host = av[1];
	port = av[2];

	clientfd = openFd::openClientFd(host, port);
	while (fgets(sendBuf, MAXBUF, stdin) != NULL)
	{
		send(clientfd, sendBuf, strlen(sendBuf), 0);
		receiveCnt = recv(clientfd, recvBuf, MAXLINE, 0);
		std::cout << recvBuf;
		for(int i = 0; i < receiveCnt; ++i)
		{
			recvBuf[i] = '\0';
		}
	}
	close(clientfd);
	exit(0);
}