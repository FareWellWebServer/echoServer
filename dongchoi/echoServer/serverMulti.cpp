#include <sys/event.h>
#include <fcntl.h>
#include <unistd.h>
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
	
	/* listen kqueue */
	int kqListen = kqueue();
	if (kqListen == -1)
		errorExit("kqueue() error");
	struct kevent clientEventListen, retEventListen;
	memset(&clientEventListen, 0, sizeof(struct kevent));
	memset(&retEventListen, 0, sizeof(struct kevent));
	
	EV_SET(&clientEventListen, listenFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	
	int retListen = kevent(kqListen, &clientEventListen, 1, NULL, 0, NULL);
	if (retListen == -1)
		errorExit("kevent()");
	if (clientEventListen.flags & EV_ERROR)
		errorExit("error flag");
	while (1)
	{
		/* accept() */
		struct addrinfo clientSockAddr;
		int connectFd(0);

		connectFd = accept(listenFd, (clientSockAddr.ai_addr), &(clientSockAddr.ai_addrlen));
		if (connectFd == -1)
			errorExit("connect() error");
		else
			std::cout << "CONNECT!! fd : " << connectFd << std::endl;
		
		/* kqueue */
		int kq = kqueue();
		if (kq == -1)
			errorExit("kqueue() error");
		struct kevent clientEvent, retEvent[10];
		memset(&clientEvent, 0, sizeof(struct kevent));
		for (int i = 0; i < 10; i++)
			memset(&(retEvent[i]), 0, sizeof(struct kevent));
		
		EV_SET(&clientEvent, connectFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		
		int ret = kevent(kq, &clientEvent, 1, NULL, 0, NULL);
		if (ret == -1)
			errorExit("kevent()");
		if (clientEvent.flags & EV_ERROR)
			errorExit("error flag");

		/* 통신 */

		while (1)
		{
			struct timespec timeout = { 0, 0 };
			ret = kevent(kq, NULL, 0, retEvent, 10, &timeout);
			if (ret == -1)
				errorExit("kevent() 2");
			else if (ret == 0)
				break ;
			else
			{
				for (int i = 0; i < 10; i++)
				{
					if (retEvent[i].filter & EVFILT_READ)
					{
						/* recv, send */
						char buf[1024];
						std::cout << "Can accept!" << connectFd << " | " << i << std::endl;
						recv(connectFd, buf, 1024, 0);
						std::cout << buf << std::endl;
						buf[0] = '\0';
						int sendFile = open("./index.html", O_RDONLY);
						int readLength = read(sendFile, buf, 1024);
						while(readLength)
						{
							send(connectFd, buf, 1024, 0);
							readLength = read(sendFile, buf, 1024);
						}
					}
				}
			}
		}

	}







}