// #include <unistd.h> /* execve, dup, dup2, pipe, read, write */
// #include <sys/socket.h> /* AF_INET, SOCK_STREAM, gai_strerror, socket, accept, listen, send, recv, bind, connect, getaddrinfo, freeaddrinfo, setsockopt, getsockname */
// #include <cstring> /* memset, strerror */
// #include <iostream>
// #include <fstream>
#include "server.hpp"

Server::Server() : kq_(kqueue()) {}
Server::~Server()
{
	ClearClientNode();
	ClearServerNode();
}

/* privave functions */
void Server::Echo( int connfd )
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
		std::cout << buf;
		buf[n] = '\0';
		send(connfd, buf, n, 0);
		sleep(3);
		return ;
	}
}
void Server::DeleteClientNode( const std::string& host, const std::string& port, int fd )
{
	std::set<struct Node*>::iterator it = clients_.begin();

	for(; it != clients_.end(); ++it)
	{
		if ((*it)->host == host && \
			(*it)->port == port && \
			(*it)->fd == fd)
		{
			close((*it)->fd);
			delete(*it);
			clients_.erase(*it);
			std::cout << "Disconnected to (" << host << ", " << port << ")\n";
			return;
		}
	}
	std::cerr << "Error: DeleteClientNode(), no such client\n";
}
bool Server::IsListenFd( int fd )
{
	std::set<struct Node*>::iterator it = server_.begin();
	for(; it != server_.end(); ++it)
	{
		if (fd == (*it)->fd)
			return true;
	}
	return false;
}
struct Node* Server::NewNode(const std::string& host, const std::string& port, int fd, int status)
{
	struct Node* new_node = new struct Node;

	new_node->host = host;
	new_node->port = port;
	new_node->fd = fd;
	new_node->status = status;
	return new_node;
}
void Server::ClearServerNode( void )
{
	std::set<struct Node*>::iterator it = server_.begin();
	for(; it != server_.end(); ++it)
	{
		close((*it)->fd);
		delete(*it);
	}
	server_.clear();
}
void Server::ClearClientNode( void )
{
	std::set<struct Node*>::iterator it = clients_.begin();
	for(; it != clients_.end(); ++it)
	{
		close((*it)->fd);
		delete(*it);
	}
	clients_.clear();
}


/* public functions */
void Server::Listen(const std::string& host, const std::string& port)
{
	if (server_.size() == MAXLISTEN)
	{
		std::cerr << "Error: full of listening\n";
		return;
	}

	struct addrinfo hints, *listp, *p;
	int listenfd, status, optval = ENABLE;
	memset(&hints, 0, sizeof(struct addrinfo));

	/*
	SOCK_STREAM
	Use ai_family, ai_socktype, ai_protocol for socket()
	*/
	hints.ai_socktype = SOCK_STREAM;

	/*
	AI_PASSIVE
	Use returned sockaddr(ai_addr) for bind() and listen()
	In short, use sockaddr(ai_addr) for server side.
	*/
	hints.ai_flags = AI_PASSIVE;

	/*
	AI_ADDRCONFIG
	If the system only has IPv4 addresses configured,
	then getaddrinfo will only return IPv4 addresses.
	If the system only has IPv6 addresses configured,
	then getaddrinfo will only return IPv6 addresses.
	*/
	hints.ai_flags |= AI_ADDRCONFIG;
	
	/*
	AI_NUMERICSERV
	Consider `port' param as `number' (ex: 80, 4242...)
	not `service name' (ex: http, ftp...)
	*/
	hints.ai_flags |= AI_NUMERICSERV;


	/*
	AI_NUMERICHOST
	Consider `host' param as `number' (ex: 127.0.0.1)
	not `domain name' (ex: www.google.com)
	*/
	hints.ai_flags |= AI_NUMERICHOST;

	status = getaddrinfo(host.c_str(), port.c_str(), &hints, &listp);

	if (status != 0)
	{
		gai_strerror(status);
		return;
	}

	for (p = listp; p; p = p->ai_next)
	{
		listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenfd < 0)
			continue;

		/*
		Set socket option.
		SOL_SOCKET - It specifies that the options being
					set or retrieved are socket options
		SO_REUSERADDR - Even if port is already occupied by another
						socket, we can use this port again.
		optval - Defined as ENABLE
		*/
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, \
					reinterpret_cast<const void*>(&optval), \
					sizeof(int));

		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
			break;
		close(listenfd);
	}

	freeaddrinfo(listp);

	if(p == NULL)
	{
		std::cerr << "Error: socket()\n";
		return;
	}

	if (listen(listenfd, BACKLOG) < 0)
	{
		close(listenfd);
		std::cerr << "Error: listen()\n";
		return;
	}

	struct kevent event;

	EV_SET(&event, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
        std::cerr << "Error: kevent()\n";
        return;
    }

	server_.insert(NewNode(host, port, listenfd, status));
	std::cout << host << " is listening port on " << port << "\n";
}
void Server::Run( void )
{
	if (server_.size() == 0)
	{
		std::cerr << "Error: no listening port\n";
		return;
	}


	char					buffer[MAXBUF];
	char					host[MAXBUF];
	char					port[MAXBUF];
	int						connfd, flags;
	socklen_t				client_len;
	struct sockaddr_storage	client_addr;
	struct kevent			event;


	while (true)
	{
		int n = kevent(kq_, NULL, 0, events_, MAXLISTEN, NULL);
        if (n == -1) {
            std::cerr << "Error: kevent()\n";
            return;
        }
		for (int idx = 0; idx < n; ++idx)
		{
            // A read event on the socket means there is a new connection
            if (IsListenFd(events_[idx].ident) && events_[idx].filter == EVFILT_READ)
			{
                // Accept the new connection
                client_len = sizeof(client_addr);
                connfd = accept(events_[idx].ident, reinterpret_cast<struct sockaddr *>(&client_addr), &client_len);
                if (connfd == -1)
				{
                    if (errno != EWOULDBLOCK) {
                        std::cerr << "Error: accept()\n";
                        continue;
                    }
                }
				else
				{
                    // Set the new client socket to non-blocking mode
                    // flags = fcntl(connfd, F_GETFL, 0);
                    // if (flags == -1) {
                    //     std::cerr << "Error: fcntl()\n";
                    //     continue;
                    // }
                    // flags |= O_NONBLOCK;
                    // if (fcntl(connfd, F_SETFL, flags) == -1) {
                    //     std::cerr << "Error: fcntl()\n";
                    //     continue;
                    // }

                    // Set up a kevent to monitor the client socket for read events
                    EV_SET(&event, connfd, EVFILT_READ, EV_ADD, 0, 0, NULL);

                    // Register the kevent with the kernel event queue
                    if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
                        std::cerr << "Error: kevent()\n";
                        continue;
                    }

                    // Set up a kevent to monitor the client socket for write events
                    EV_SET(&event, connfd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);

                    // Register the kevent with the kernel event queue
                    if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
                        std::cerr << "Error: kevent()\n";
                        continue;
                    }
                }
				getnameinfo(reinterpret_cast<struct sockaddr *>(&client_addr), client_len,\
							host, MAXBUF,\
							port, MAXBUF,\
							0);

				clients_.insert(NewNode(host, port, connfd, 0));
				std::cout << "Connected to (" << host << ", " << port << ")\n";
            }
			else if (events_[idx].filter == EVFILT_READ)
			{
				EV_SET(&event, events_[idx].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) std::cerr << "Error: kevent()\n";
				Echo(events_[idx].ident);
				// http::doit(events_[idx].ident);
				DeleteClientNode(host, port, events_[idx].ident);
				// close(events_[idx].ident);
			}
		}
	}
}

extern char** environ;

int main(int ac, char* av[])
{
	Server server;

	std::string host("127.0.0.1");
	std::string port("8080");

	server.Listen(host, port);

	port = "4242";
	server.Listen(host, port);

	port = "4244";
	server.Listen(host, port);

	server.Run();

	return 0;
}



// void echo(int connfd);

// int main(int ac, char* av[])
// {
// 	int listenfd, connfd;
// 	socklen_t clientlen;
// 	struct sockaddr_storage client_addr;
// 	char clientHostname[MAXLINE], clientPort[MAXLINE];

// 	if (ac != 2)
// 	{
// 		std::cerr << "usage: " << av[0] << " <port>\n";
// 		exit(0);
// 	}

// 	listenfd = ft::OpenListenFd(av[1]);

// 	while (1)
// 	{
// 		clientlen = sizeof(struct sockaddr_storage);
// 		connfd = accept(listenfd, reinterpret_cast<struct sockaddr *>(&client_addr), &clientlen);
// 		getnameinfo(reinterpret_cast<struct sockaddr *>(&client_addr), clientlen,\
// 					clientHostname, MAXLINE,\
// 					clientPort, MAXLINE,\
// 					0);
// 		std::cout << "Connected to (" << clientHostname << ", " << clientPort << ")\n";
// 		echo(connfd);
// 		close(connfd);
// 	}
// 	return 0;
// }




