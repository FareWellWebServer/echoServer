#include <sys/event.h>

#include <sys/socket.h> // socket()
#include <netdb.h> //struct addrinfo
#include <exception>
#include <iostream>

void errorExit(const char *str)
{
	std::cout << str << std::endl;
	exit(1);
}

int main(int ac, char *av[])
{
	if (ac != 2)
		errorExit("argv[1]");

	/* socket() */
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenFd == -1)
		errorExit("socket() error");

	/* get address infomation */
	struct addrinfo *listenAddr, hint;
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE | AI_NUMERICSERV | AI_ADDRCONFIG;
	getaddrinfo(NULL, av[1], &hint, &listenAddr);

	/* bind() */
	if (bind(listenFd, listenAddr->ai_addr, listenAddr->ai_addrlen) == -1)
		errorExit("bind() error");

	/* listen() */
	if (listen(listenFd, 10) == -1)
		errorExit("listen() error");
	
	/* accept() */
	struct addrinfo clientSockAddr;
	int connectFd(0);

	connectFd = accept(listenFd, (clientSockAddr.ai_addr), &(clientSockAddr.ai_addrlen));
	if (connectFd == -1)
		errorExit("connect() error");
	else
		std::cout << "CONNECT!! fd : " << connectFd << std::endl;


	/* recv, send */
	char buf[1024];
	while(1)
	{
		std::cout << "Can accept!" << connectFd << std::endl;
		recv(connectFd, buf, 1024, 0);
		std::cout << buf << std::endl;
	}

}