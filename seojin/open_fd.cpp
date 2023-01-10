#include "echo_server.hpp"

namespace ft
{

int OpenClientFd(char *hostname, char *port)
{
	int clientfd;
	struct addrinfo hints, *listp, *p;

	/* Get a list of potential server addresses */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM; /* Open a connection */
	hints.ai_flags = AI_NUMERICSERV; /* using a numeric port arg */
	hints.ai_flags |= AI_ADDRCONFIG; /* Recommended for connections */
	getaddrinfo(hostname, port, &hints, &listp);


	/* Walk the list for one that we can successfully connect to */
	for (p = listp; p; p = p->ai_next)
	{
		/* Create a socket descriptor */
		clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (clientfd < 0)
			continue;;

		/* Connect to server */
		if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
			break; /* Success */
		close(clientfd); /* Connect failed, try another */
	}

	/* Clean up */
	freeaddrinfo(listp);
	if (!p) /* All connects failed */
		return -1;
	else
		return clientfd; /* The last connect succeeded */
}


int OpenListenFd(char *port)
{
	struct addrinfo hints, *listp, *p;
	int listenfd, optval = 1;

	/* Get a list of potential server addresses */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_flags |= AI_NUMERICSERV;
	getaddrinfo(NULL, port, &hints, &listp);

	/* Wlak the list for one that we can bind to */
	for (p = listp; p; p = p->ai_next)
	{
		listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenfd < 0)
			continue;
		
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void *>(&optval), sizeof(int));

		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
			break;
		close(listenfd);
	}

	freeaddrinfo(listp);
	if (!p)
		return -1;


	if (listen(listenfd, 1024) < 0)
	{
		close(listenfd);
		return -1;
	}

	return listenfd;
}


};