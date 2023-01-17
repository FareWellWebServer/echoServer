/*
socket() -> 연결되지 않은 새로운 소켓 생성
bind() -> socket에 로컬 포트 번호 할당
listen() -> 커넥션을 받기 위해 로컬 소켓에 허용함을 표시
kqueue(), kevent() -> 여러 클라이언트를 이벤트(recv, send)
                        기반으로 커널 딴에서 처리함
accept() -> 누군가 로컬 포트에 커낵션을 맺기를 기다림
recv() -> request 받음
send() -> response 보냄
close() -> 소켓 닫음

EX)
1. 새로운 socket 생성(socket)
2. 80 포트로 socket을 묶는다.
3. socket 커낵션을 허가함(listen)
4. 클라이언트 커넥트 기다림(kqueue, accept)

- 한 클라이언트와 커낵션됨 -
5. 요청(http request)을 기다림(recv)

- request 받음 -
6. request 처리
7. response 보냄(send)
8. 커낵션을 닫는다(close)
*/

#include "Server.hpp"

Server::Server(int port) : port_(port) {}

Server::~Server(void) {}

std::string Server::GetIp(void) {
  std::string ip = std::string(inet_ntoa(address_.sin_addr));
  std::string port = std::to_string(ntohs(address_.sin_port));
  return ip + ":" + port;
}

void Server::ChangeEvents(std::vector<struct kevent>& change_list, int socket,
                          int16_t filter, uint16_t flags, uint32_t fflags,
                          intptr_t data, void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, socket, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}

void Server::DisconnectClient(int client_fd,
                              std::map<int, std::string>& clients) {
  std::cerr << "Client disconnected" << std::endl;
  close(client_fd);
  clients.erase(client_fd);
}

int Server::Set(void) {
  server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd_ < 0) throw std::runtime_error("Failed to create socket");

  address_.sin_family = AF_INET;
  address_.sin_port = htons(port_);
  address_.sin_addr.s_addr = inet_addr("127.0.0.1");

  int optval = 1;
  setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  int bind_result = bind(server_fd_, (sockaddr*)&address_, sizeof(address_));
  if (bind_result < 0)
    throw std::runtime_error("Failed to bind socket to address");

  int listen_result = listen(server_fd_, 1024);
  if (listen_result < 0) throw std::runtime_error("Failed to listen on socket");

  return 0;
}

int Server::Run(void) {
  int kq = kqueue();
  if (kq == -1) throw std::runtime_error("Failed to init kqueue");

  std::map<int, std::string> clients;
  std::vector<struct kevent> change_list;
  struct kevent event_list[128];

  ChangeEvents(change_list, server_fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
               NULL);

  int new_events = 1;
  struct kevent* curr_event;

  while (42) {
    new_events =
        kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
    if (new_events == -1) throw std::runtime_error("Failed kevent()");

    change_list.clear();

    for (int i = 0; i < new_events; ++i) {
      curr_event = &event_list[i];

      if (curr_event->flags & EV_ERROR) {
        if (curr_event->ident == static_cast<uintptr_t>(server_fd_))
          throw std::runtime_error("Server socket error");
        else
          DisconnectClient(curr_event->ident, clients);
      } else if (curr_event->filter == EVFILT_READ) {
        if (curr_event->ident == static_cast<uintptr_t>(server_fd_)) {
          socklen_t client_len = sizeof(address_);
          int client_fd = accept(server_fd_, (sockaddr*)&address_, &client_len);
          if (client_fd == -1)
            PrintError("Failed to accept incoming connection");

          fcntl(client_fd, F_SETFL, O_NONBLOCK);

          std::cout << "Accepted connection from " << GetIp() << std::endl;

          ChangeEvents(change_list, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE,
                       0, 0, NULL);
          // ChangeEvents(change_list, client_fd, EVFILT_WRITE, EV_ADD |
          // EV_ENABLE,
          //              0, 0, NULL);
          clients[client_fd] = "";
        } else if (clients.find(curr_event->ident) != clients.end()) {
          char buf[BUFSIZE];
          ssize_t bytes_received = recv(curr_event->ident, buf, sizeof(buf), 0);
          if (bytes_received <= 0) {
            if (bytes_received < 0)
              std::cerr << "Failed to receive data from client" << std::endl;
            DisconnectClient(curr_event->ident, clients);
          } else {
            buf[bytes_received] = '\0';
            clients[curr_event->ident] += buf;

            // std::cout << "request from " << GetIp() << ": " << buf <<
            // std::endl;

            std::map<int, std::string>::iterator it =
                clients.find(curr_event->ident);
            if (it != clients.end()) {
              if (clients[curr_event->ident] != "") {
                Response response(buf);
                response.ResponseHandler();
                std::string response_str = response.GetResponse();
                send(curr_event->ident, response_str.c_str(),
                     response_str.size(), 0);
              }
              // DisconnectClient(curr_event->ident, clients);
              // clients[curr_event->ident].clear();
            }
          }
        }
      }
    }
  }
  return 0;
}

void CheckArgument(int ac, char** av) {
  if (ac < 2) throw std::runtime_error("Few argument error");
  for (std::size_t i = 0; av[1][i] != 0; i++) {
    if (!std::isdigit(av[1][i])) throw std::runtime_error("Port is not number");
  }
}

int PrintError(const std::string str) {
  std::cerr << str << std::endl;
  return 1;
}
