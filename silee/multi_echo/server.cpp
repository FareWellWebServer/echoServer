#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <vector>

using namespace std;

void exit_with_perror(const string& msg)
{
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

void set_events(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

void disconnect_client(int client_fd, map<int, string>& clients)
{
	cout << "client disconnected: " << client_fd << endl;
	close(client_fd);
	clients.erase(client_fd);
}

int main()
{
	/* init server socket and listen */
	int server_socket;
	struct sockaddr_in server_addr;

	//소켓 생성.
	if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		exit_with_perror("socket() error\n" + string(strerror(errno)));

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(8080);
	//생성된 소켓에 ip주소와 port 번호 binding.
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		exit_with_perror("bind() error\n" + string(strerror(errno)));

	//통신 할 준비가 되어있음. 듣기 시작.
	if (listen(server_socket, 5) == -1)
		exit_with_perror("listen() error\n" + string(strerror(errno)));
	//생성된 소켓의 상태를 변경한다.  -> why?
	
	/* 
	int fcntl(int fd, int cmd, ...argv)
		<int fd>
			파일의 속성 조회 혹은 속성을 변경할 파일 디스크립터
		<int cmd>
			F_GETFL : fd의 속성을 반환값으로 가져옴.
			F_SETFL : fd 속성을 세번째 인수로 반다 설정한다. 
				(O_APPEND, O_NONBLOCK, O_SYNC, O_DSYNC, O_RSYNC, O_FSYNC, O_ASYNC 속성만 변경)
			(그리고 F_GETOWN, F_SETOWN ,F_DUPFD, F_DUPFC_CLOEXEC, F_GETFD, F_SETFD 등)
	*/

	fcntl(server_socket, F_SETFL, O_NONBLOCK);
	/* init kqueue */
	//커널에 새로운 event queue를 만들고 거기에 접근이 가능한 sd 반환. 반환된 값은 kevent() 함수에서 이벤트 등록, 모니터링 시 사용됨./
	int kq;
	if ((kq = kqueue()) == -1)
		exit_with_perror("kqueue() error\n" + string(strerror(errno)));

	/*
	int kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout);
	- kq로 전달된 kqueue에 새로 모니터링할 이벤트를 등록하고, 발생하여 아직 처리되지 않은(pending 상태인) 이벤트의 개수를 return한다. 

	<kevent 구조체>
		struct kevent {
			uintptr_t ident;         identifier for this event  //체크할 fd
				이벤트의 source를 식별
			int16_t   filter;        filter for event 
				@ident의 fd를 모니터링. 읽기/쓰기/~~ 가 가능하다면 해당 event를 change_list에 포함.
			uint16_t  flags;         action flags for kqueue 
				@event가 change_list에 포함될때 (return될때) 사용되는 flag
					EV_ADD : kqueue 에 event emdfhr
					EB_ENABLE : kevent가 값을 change_list에 넣을 수 있도록 활성화
					EV_DISABLE : kevent가 event를 담을 수 없음
					EV_DELETE : kqueue에서 event 삭제
			uint32_t  fflags;        filter flag value 
			intptr_t  data;          filter data value 
			void      *udata;        opaque user data identifier
		};
	
	<const struct kevent *changelist>
		change list는 kevent의 구조체 배열로, 배열 내 저장된 구조체들을 kqueue에 등록한다
	<int nchanges>
		등록할 kevent 구조체의 개수.
	<struct kevent *eventlist>
		발생 한 event가 return (들어갈) 배열이다
	<int nevents>
		발생 한 event가 return 될 배열의 개수
	<const struct timespec *timeout>

	return : 
		발생하여 changelist에 담긴 (아직 처리되지 않음) 이벤트의 "개수" 를 리턴한다.	

	*/

	map<int, string> clients; // map for client socket:data
	vector<struct kevent> change_list; // kevent vector for changelist
	struct kevent event_list[8]; // kevent array for eventlist

	/* add event for server socket */
	set_events(change_list, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	/*
	void set_events(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata){
		struct kevent temp_event;

		//ev_set 함수는 kevnent 구조체를 초기화 하는데 사용됨.
		EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
		change_list.push_back(temp_event);
		}
	*/
	cout << "echo server started" << endl;

	int new_events;
	struct kevent* curr_event;
	while (1)
	{
		/*  apply changes and return new events(pending events) 
		
			changelist에 있는 녀석(kevent구조체들이 
			kqueue에 등록됨. 그리고 처리가 가능한 녀석들은 event_list에 저장.
		
			event_list에 event들이 담겨 있을 것.
		
		 		처리되지 않은 event의 개수 = new_event.*/
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
		if (new_events == -1)
			exit_with_perror("kevent() error\n" + string(strerror(errno)));

		change_list.clear(); // clear change_list for new changes

		for (int i = 0; i < new_events; ++i)
		{
			//처리되지 않은 녀석들을 돌며 처리하기 위해 curr에 연결.
			curr_event = &event_list[i];

			/* check error event return */
			// & 비트연산인데 어떻게되는건지..?.. 
			//암튼 에러일때.
			if (curr_event->flags & EV_ERROR)
			{
				if (curr_event->ident == server_socket)
					exit_with_perror("server socket error");
				else
				{
					cerr << "client socket error" << endl;
					disconnect_client(curr_event->ident, clients);
				}
			}
			//EVFILT_READ : 읽을 수 있는 데이터가 존재할때. (ev_set할때 모두 EVFILT_READ로 초기화했었음)
			else if (curr_event->filter == EVFILT_READ)
			{
				//읽을 수 있는 상태이고, kevent에 담긴 녀석이 서버 소켓일때 -> 
				// evenT LIST는 서버소켓을 제외하고 비어있는 상태이므로 클라이언트를 받아야함.
				if (curr_event->ident == server_socket)
				{
					/* accept new client */
					int client_socket;
					if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
						exit_with_perror("accept() error\n" + string(strerror(errno)));
					cout << "accept new client: " << client_socket << endl;
					fcntl(client_socket, F_SETFL, O_NONBLOCK);

					/* add event for client socket - add read && write event */
					set_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					set_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					clients[client_socket] = "";
				}

				//서버소켓이 아니고, ident가 존재한다면 받아온 클라이언트의 fd에서 읽어오고 버퍼에 저장하고
				// map으로 선언된 녀석에 저장.
				else if (clients.find(curr_event->ident)!= clients.end())
				{
					/* read data from client */
					char buf[1024];
					int n = read(curr_event->ident, buf, sizeof(buf));

					if (n <= 0)
					{
						if (n < 0)
							cerr << "client read error!" << endl;
						disconnect_client(curr_event->ident, clients);
					}
					else
					{
						buf[n] = '\0';
						clients[curr_event->ident] += buf;
						cout << "received data from " << curr_event->ident << ": " << clients[curr_event->ident] << endl;
					}
				}
			}

			//EVFILT_WRITE : 해당 fd에 write가 가능할 때.

			//183줄에서 클라이언트 소켓에 대해서 읽기, 쓰기가 모두 가능하므로 소켓에 대한 쓰기가 가능한 녀석에 접근.
			else if (curr_event->filter == EVFILT_WRITE)
			{
				/* send data to client */
				//해당 ident를 가진 녀석을 iterator를 통해 찾음.
				map<int, string>::iterator it = clients.find(curr_event->ident);
				//찾았다면
				if (it != clients.end())
				{
					//널이 아니라면
					if (clients[curr_event->ident] != "")
					{
						int n;
						//해당 소켓에 작성해주면 된다.
						if ((n = write(curr_event->ident, clients[curr_event->ident].c_str(),
										clients[curr_event->ident].size()) == -1))
						{
							cerr << "client write error!" << endl;
							disconnect_client(curr_event->ident, clients);  
						}
						//write 실패 시 비워줌.
						else
							clients[curr_event->ident].clear();
					}
				}
			}
		}

	}
	return (0);
}