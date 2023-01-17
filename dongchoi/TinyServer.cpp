#include "TinyServer.hpp"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

TinyServer::TinyServer(char **port, int portCount) {
  /* get address infomation */
  struct addrinfo hint;
  memset(&hint, 0, sizeof(struct addrinfo));
  hint.ai_family = AF_INET;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_flags = AI_PASSIVE | AI_NUMERICSERV | AI_ADDRCONFIG | AI_CANONNAME;

  /* kqueue */
  kq_ = kqueue();
  if (kq_ == -1) throw std::runtime_error("TinyServer : kq create failed");

  for (int i = 0; i < portCount; i++) {
    /* socket() */
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
      throw std::runtime_error("TinyServer : socket create failed");

    struct addrinfo *listen_addr;
    getaddrinfo(NULL, port[i], &hint, &listen_addr);

    std::cout << listen_addr->ai_canonname << std::endl;
    /* event enroll listen FD */
    EV_SET(&event_setting_, listen_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           0);
    if (kevent(kq_, &event_setting_, 1, NULL, 0, NULL) == -1) {
      throw std::runtime_error("TinyServer : kevent failed in Construct");
    }
    if (event_setting_.flags & EV_ERROR) {
      throw std::runtime_error("TinyServer : event someting wrong");
    }
    Bind(listen_fd, listen_addr->ai_addr, listen_addr->ai_addrlen);
    listen_fd_.push_back(listen_fd);

    listen_addr_.push_back(listen_addr);
  }
}

TinyServer::~TinyServer() {
  for (std::vector<struct addrinfo *>::iterator itr = listen_addr_.begin();
       itr != listen_addr_.end(); itr++)
    freeaddrinfo(*itr);
}

void TinyServer::Listen() {
  for (std::vector<int>::iterator itr = listen_fd_.begin();
       itr != listen_fd_.end(); itr++) {
    if (listen(*itr, listen_backlog_cnt_) == -1)
      throw std::runtime_error("TinyServer : listen function failed");
  }
}

void TinyServer::Bind(int listen_fd, const struct sockaddr *ai_addr,
                      socklen_t ai_addrlen) {
  if (bind(listen_fd, ai_addr, ai_addrlen) == -1)
    throw std::runtime_error("TinyServer : bind function failed");
  // if (bind(listen_fd_, listen_addr_->ai_addr, listen_addr_->ai_addrlen) ==
  // -1)
}

// accept 할 때 client.addr 정보 저장 해놓을 필요 없을까
void TinyServer::Accept(int listen_fd) {
  std::cout << "Accept() listen FD : " << listen_fd << std::endl;
  // struct addrinfo clientSockAddr;
  struct sockaddr *clientSockAddr;
  socklen_t len = sizeof(clientSockAddr);
  int connect_fd(0);

  connect_fd = accept(listen_fd, clientSockAddr, &len);
  if (connect_fd == -1)
    throw std::runtime_error("TinyServer : accept function failed");
  else
    std::cout << "[Connect Success] fd : " << connect_fd << std::endl;
  clients_fd_.push_back(connect_fd);
  EV_SET(&event_setting_, connect_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
  if (kevent(kq_, &event_setting_, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("TinyServer : kevent failed in Accept()");
  }
}

// event_trigger size는 어떻게 설정해야 하는가. clents 연결 끊기면 제거해주면
// 되는가
void TinyServer::Action() {
  struct kevent event_trigger[listen_fd_.size() + clients_fd_.size() + 1];
  char buffer[rdwr_buf_size_];
  int exist_events = kevent(kq_, NULL, 0, event_trigger,
                            listen_fd_.size() + clients_fd_.size() + 1, 0);
  std::cout << "exist_envets : " << exist_events << std::endl;
  if (exist_events == -1) {
    throw std::runtime_error("TinyServer : kevent failed in Action()");
  }
  for (int i = 0; i < exist_events; i++) {
    int accept_complete = 0;
    for (std::vector<int>::iterator itr = listen_fd_.begin();
         itr != listen_fd_.end(); itr++) {
      if (event_trigger[i].ident == *itr &&
          event_trigger[i].filter == EVFILT_READ) {
        std::cout << "iter listen FD : " << *itr << std::endl;
        std::cout << "!!!" << std::endl;
        Accept(*itr);
        std::cout << "@@@@" << std::endl;
        accept_complete = 1;
        break;
      }
    }
    if (accept_complete == 1) continue;

    if (event_trigger[i].filter == EVFILT_READ) {
      // already connection
      const int client_fd = event_trigger[i].ident;
      std::cout << "client_fd : " << client_fd << std::endl;
      if (read(client_fd, buffer, rdwr_buf_size_) == 0) {
        close(client_fd);
        continue;
      }
      // GetRequest(client_fd);
      int request_fd(0);
      if (!strncmp(buffer, "GET", 3))
        request_fd = open("./index.html", O_RDONLY);
      else
        request_fd = open("./error.html", O_RDONLY);
      int read_cnt = read(request_fd, buffer, rdwr_buf_size_);
      write(client_fd, buffer, read_cnt);
      // ProcessRequest();
      // SendResponse();
    }
  }
}
void TinyServer::run() {
  // Bind();
  Listen();
  while (1) {
    std::cout << "Action()" << std::endl;
    Action();
  }
}

/*-------------------------------------------------------------------*/

// #include <fstream>
// #include <string.h>
// #include <fcntl.h>
// #include <unistd.h>

// const char *write_file_name_temp;

// void GetRequest(int client_fd)
// {
// 	std::string file_name("request_http_header");
// 	file_name.append(std::to_string(client_fd)); // C++11
// 	write_file_name_temp = file_name.c_str();
// 	int write_file_fd = open(file_name.c_str(), O_WRONLY | O_CREAT);
// 	if (write_file_fd == -1)
// 		throw std::runtime_error("GetRequest : write file open error");
// 	char buf[1024];
// 	while(read(client_fd, buf, 1024))
// 		write(write_file_fd, buf, 1024);

// }

// void ProcessRequest()
// {

// }

// void SendResponse()
// {

// }
