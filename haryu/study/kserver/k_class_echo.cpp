#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h> // atoi

#include <ctime>
#include <iostream>
#include <map>
#include <vector>
#include <string>

#define String std::string
#define COUT std::cout 
#define CEND std::endl
#define CER std::cerr
#define VEC std::vector
#define MAP std::map

#ifndef EVENT_MAX
# define EVENT_MAX 8
#endif

#ifndef REUSE
# define REUSE 1
#endif 

#ifndef LINGER
# define LINGER 1
#endif

#ifndef DEFAULTLOCATION
# define DEFAULTLLOCATION "/Users/haryu/workspace/echoServer/haryu/study/kserver"
#endif 


// 목표 1 : 페이지 전달 서버 구축하기
// 목표 2 : 다중 포트 상태로 값이 들어오면 서버 클래스가 자동으로 멀티 포트로 운영됨 
// 목표 3 : 접근하면 index.html 전달하기 
// 목표 4 : index_comp.html 전닳시키고 커낵션 종료 시켜보기

class Server {
private : 
	int serverSocket;
	struct sockaddr_in serverAddr;
	int kq;
	MAP<int, String> clients;
	VEC<struct kevent> changeList;
	struct kevent eventList[EVENT_MAX];
	String msg;

	int setSocketOptions();
	void exitWithPerror(const String& msg);
	void changeEvents(VEC<struct kevent>& changeList, uintptr_t ident, int16_t filter, uint16_t flags, uint16_t fflags, intptr_t data, void* udata);
	void disconnectClients(int clientFd, MAP<int, String>& clients);
	void readEvent();
	void writeEvent(String msg);
public :
	Server(int portNumber, char **port);
	void bind();
	void listen();
	void turnOn();
	void showYourSelf();
};

int main(int ac, char **av) {
	Server myServer(ac, av);
	
	myServer.bind();
	myServer.listen();
	myServer.showYourSelf();
	myServer.turnOn();
	
	return 0;
}