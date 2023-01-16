#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h> // atoi
#include <unistd.h>

#include <ctime>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>

#define String std::string
#define COUT std::cout 
#define CEND std::endl
#define CER std::cerr
#define VEC std::vector
#define MAP std::map

#ifndef EVENT_MAX
# define EVENT_MAX 100
#endif

#ifndef REUSE
# define REUSE 1
#endif 

#ifndef LINGER
# define LINGER 1
#endif

#ifndef DEFAULTLOCATION
# define DEFAULTLLOCATION "/Users/haryu/workspace/echoServer/haryu/study/kserver/"
#endif 

#define HTTP "httpmsg"
#define INDEX "index.html"
#define INDEXC "index_comp.html"
#define SRC "/src/"

// 목표 1 : 페이지 전달 서버 구축하기
// 목표 2 : 다중 포트 상태로 값이 들어오면 서버 클래스가 자동으로 멀티 포트로 운영됨 
// 목표 3 : 접근하면 index.html 전달하기 
// 목표 4 : index_comp.html 전닳시키고 커낵션 종료 시켜보기

struct host {
	int serverSocket;
	struct sockaddr_in serverAddr;
};

class Server {
private : 
	int * serverSocket;
	struct sockaddr_in * serverAddr;
	int portnum;
	int kq;
	MAP<int, String> clients;
	VEC<struct kevent> changeList;
	struct kevent eventList[EVENT_MAX];
	String msg;

	void exitWithPerror(const String& msg) {
		CER << msg << CEND;
		exit(EXIT_FAILURE);
	}
	void changeEvents(VEC<struct kevent>& changeList, uintptr_t ident, int16_t filter, uint16_t flags, uint16_t fflags, intptr_t data, void* udata) {
		struct kevent tempEvent;

		EV_SET(&tempEvent, ident, filter,  flags, fflags, data, udata);
		changeList.push_back(tempEvent);
	}
	void disconnectClients(int clientFd, MAP<int, String>& clients) {
		std::cout << "client disconnected : " << clientFd << std::endl;
		close(clientFd);
		clients.erase(clientFd);
	}
public :
	Server(int portNumber, char **port) : msg("") {
		if (portNumber == 1) { 
			COUT << "./k_http_server {port 1} {port 2} ... " << CEND;
			exit (1);
		}
		serverSocket = new int[portNumber];
		if(serverSocket == nullptr) {
			CER << "constructor error" << CEND;
			exit (1);
		}
		serverAddr = new sockaddr_in[portNumber];
		if (serverAddr == nullptr) {
			CER << "constructor error" << CEND;
			exit (1);
		}
		for (int i = 0; i + 1 < portNumber; i++) {
			serverSocket[i] = socket(PF_INET, SOCK_STREAM, 0);
			if (serverSocket[i] == -1) { exitWithPerror("socket() error\n" + String(strerror(errno))); }

			memset(&serverAddr[i], 0, sizeof(serverAddr[i]));
			serverAddr[i].sin_family = AF_INET;
			serverAddr[i].sin_addr.s_addr = htonl(INADDR_ANY);
			serverAddr[i].sin_port = htons(atoi(port[i + 1]));
		}
		portnum = portNumber - 1;
	}
	~Server() {
		delete[] serverSocket;
		delete[] serverAddr;
	}
	void	myBind() {
		int ret;

		for (int i = i; i < portnum; i++) {
			ret = bind(serverSocket[i], reinterpret_cast<struct sockaddr *>(&serverAddr[i]), sizeof(serverAddr[i]));
			if (ret == -1) { exitWithPerror("bind() error\n" + String(strerror(errno))); }
		}
		return ;
	}
	void	myListen() {
		int ret;

		for (int i = 0; i < portnum; i++) {
			ret = listen(serverSocket[i], 10);
			if (ret == -1) { exitWithPerror("listen() error\n" +  String(strerror(errno))); }
			fcntl(serverSocket[i], F_SETFL, O_NONBLOCK);
		}
	}
	int		setSockopt(int val , int fd) {
		int ret;
		
		if (val == SO_REUSEADDR) {
			int bf;

			for (int i = 0; i < portnum; i++) {
				ret = setsockopt(serverSocket[i], SOL_SOCKET, SO_REUSEADDR, &bf, sizeof(bf));
				if (ret == -1) { exitWithPerror("setsockopt(SO_REUSERADDR) error\n" + String(strerror(errno))); }
				COUT << "Socket(" << serverSocket[i] << ")setting is succesed : SO_REUSEADDR" << CEND;
			}
			return (0);
		}
		else if (val == SO_LINGER) {
			struct linger loption;

			loption.l_onoff = 1;
			loption.l_linger = 0;

			ret = setsockopt(fd, SOL_SOCKET, SO_LINGER, &loption, sizeof(loption));
			if (ret == -1) { exitWithPerror("setsockopt(SO_LINGER) error\n" + String(strerror(errno))); }
			COUT << "Socket setting is succesed : SO_LINGER(" << fd <<  ")" << CEND;
			return (0);
		}
		else {
			CER << "Socket setting is failed." << CEND;
			return (-1);
		}
		
	}
	bool	IsServer(int fd) {
		int ret = 0;

		for (int i = 0; i < portnum; i++) {
			if (serverSocket[i] == fd)
				ret += 1 ;
		}
		return (ret);
	}
	void	turnOn() {
		kq = kqueue();
		if (kq == -1) { exitWithPerror("kqueue() error\n" + String	(strerror(errno))); }

		for (int i = 0; i < portnum; i++) {
			changeEvents(changeList, serverSocket[i], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			COUT << "K-Page Server Started : " << serverSocket[i] << CEND;
		}
		

		int newEvents;
		struct kevent * currEvent;
		while(true) {
			newEvents = kevent(kq, &changeList[0], changeList.size(), eventList, EVENT_MAX, nullptr);
			if (newEvents == -1) { exitWithPerror("kevent() error\n" + String(strerror(errno))); }

			changeList.clear();

			for (int i = 0; i < newEvents; ++i) {
				currEvent = &eventList[i];
				if (currEvent->flags & EV_ERROR) {
					for (int i = 0; i < portnum; i ++) {
						if (currEvent->ident == serverSocket[i])
							exitWithPerror("server socket error");
						else {
							CER << "clients socket error" << CEND;
							disconnectClients(currEvent->ident, clients); 
						}
					}
				}
				else if (currEvent->filter == EVFILT_READ) {
					if (IsServer(currEvent->ident)) {
						/* Accept new client */
						int clientSocket(accept(currEvent->ident, NULL, NULL));
						if (clientSocket == -1) { exitWithPerror("accept() error\n" + String(strerror(errno))); }
						COUT << "accept new client : " << clientSocket << CEND;
						fcntl(clientSocket, F_SETFL, O_NONBLOCK);

						/* add event for client socket - add read && write event */
						changeEvents(changeList, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE | EV_EOF, 0, 0, NULL);
						changeEvents(changeList, clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
						setSockopt(SO_LINGER,  clientSocket);
						clients[clientSocket] = "";
					}
					else if (clients.find(currEvent->ident) != clients.end()) {
						/* read data from clients */
						char buf[1024];
						int n = recv(currEvent->ident, buf, sizeof(buf), 0);
						buf[n] = '\0';
						clients[currEvent->ident].append(buf);
						COUT << "received data from " << currEvent->ident << ":\n" << clients[currEvent->ident] << CEND;
					}
				}
				else if (currEvent->filter == EVFILT_WRITE) {
					/* send Data to client*/
					std::map<int, String>::iterator it = clients.find(currEvent->ident);
					if (it != clients.end()) {
						int n;
						n = send(currEvent->ident, msg.c_str(), strlen(msg.c_str()), MSG_DONTWAIT);
						clients[currEvent->ident].clear();
						disconnectClients(currEvent->ident, clients);
					}
				}
			}
		}
	}
	void	showYourSelf() {
		for (int i = 0; i < portnum; i++) {
			COUT << "Server Socket : " << this->serverSocket[i] << CEND;
			COUT << "Server Port : " << ntohs(this->serverAddr[i].sin_port) << CEND;
		}
	}

	void	fileSetting(const char * path) {
		std::ifstream httpmsg;
		std::ifstream entity;
		String entityloc(DEFAULTLLOCATION);

		httpmsg.open(String(DEFAULTLLOCATION).append(HTTP), std::ifstream::in);
		entityloc.append(path);

		COUT << entityloc << CEND;

		entity.open(entityloc.c_str(), std::ifstream::in);

		int lengthFirst;
		int lengthSecond;

		httpmsg.seekg(0, httpmsg.end);
		lengthFirst = httpmsg.tellg();
		httpmsg.seekg(0, httpmsg.beg);

		entity.seekg(0, entity.end);
		lengthSecond = entity.tellg();
		entity.seekg(0, entity.beg);

		char * buffer1 = new char[lengthFirst + 1];
		char * buffer2 = new char[lengthSecond + 1];
		buffer1[lengthFirst] = '\0';
		buffer2[lengthSecond] = '\0';

		httpmsg.read(buffer1, lengthFirst);
		entity.read(buffer2, lengthSecond);

		msg.append(buffer1);
		msg.push_back(static_cast<char>(13));
		msg.append("\n");
		msg.append(buffer2);
	}
};

int main(int ac, char **av) {
	Server myServer(ac, av);
	
	myServer.myBind();
	myServer.myListen();
	myServer.showYourSelf();
	myServer.setSockopt(SO_REUSEADDR, 0);
	myServer.fileSetting(INDEX);
	myServer.turnOn();
	
	return 0;
}