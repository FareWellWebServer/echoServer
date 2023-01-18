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

int Server::GetServerIdx(int sock) {
  std::vector<int>::iterator it =
      std::find(server_fds_.begin(), server_fds_.end(), sock);
  int i = it - server_fds_.begin();

  return i;
}

void Server::ChangeEvents(int socket, int16_t filter, uint16_t flags,
                          uint32_t fflags, intptr_t data, void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, socket, filter, flags, fflags, data, udata);
  change_list_.push_back(temp_event);
}

void Server::DisconnectClient(int client_fd) {
  std::cerr << "Client disconnected" << std::endl;
  close(client_fd);
  clients_.erase(client_fd);
}

int Server::Set(void) {
  int optval = 1;
  int sock;
  sockaddr_in address;

  for (int i = 0; i < 5; ++i) {
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) throw std::runtime_error("Failed to create socket");

    address.sin_family = AF_INET;
    address.sin_port = htons(port_ + i);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    server_fds_.push_back(sock);
    address_.push_back(address);

    fcntl(sock, F_SETFL, O_NONBLOCK);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    int bind_result = bind(sock, (sockaddr*)&address, sizeof(address));
    if (bind_result < 0)
      throw std::runtime_error("Failed to bind socket to address");

    int listen_result = listen(sock, 1024);
    if (listen_result < 0)
      throw std::runtime_error("Failed to listen on socket");
  }
  return 0;
}

int Server::Run(void) {
  int kq = kqueue();
  if (kq == -1) throw std::runtime_error("Failed to init kqueue");

  struct kevent event_list[128];

  for (std::size_t i = 0; i < server_fds_.size(); ++i)
    ChangeEvents(server_fds_[i], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

  int new_events = 1;
  struct kevent* curr_event;

  while (42) {
    new_events =
        kevent(kq, &change_list_[0], change_list_.size(), event_list, 8, NULL);
    if (new_events == -1) throw std::runtime_error("Failed kevent()");

    change_list_.clear();

    for (int i = 0; i < new_events; ++i) {
      curr_event = &event_list[i];

      if (curr_event->flags & EV_ERROR)
        HandleErrorEvent(curr_event->ident);
      else if (curr_event->filter == EVFILT_READ)
        HandleReadEvent(curr_event->ident);
    }
  }
  return 0;
}

int Server::IsServer(uintptr_t ident) {
  for (std::size_t i = 0; i < server_fds_.size(); ++i) {
    if (ident == static_cast<uintptr_t>(server_fds_[i])) return 1;
  }
  return 0;
}

int Server::HandleErrorEvent(uintptr_t ident) {
  if (IsServer(ident))
    throw std::runtime_error("Server socket error");
  else
    DisconnectClient(ident);
  return 0;
}

int Server::HandleReadEvent(uintptr_t ident) {
  if (IsServer(ident)) {
    int i = GetServerIdx(ident);
    socklen_t client_len = sizeof(address_[i]);
    int client_fd =
        accept(server_fds_[i], (sockaddr*)&address_[i], &client_len);
    if (client_fd == -1)
      return (PrintError("Failed to accept incoming connection"));

    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    ChangeEvents(client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    clients_[client_fd] = "";
  } else if (clients_.find(ident) != clients_.end()) {
    char buf[BUFSIZE];
    ssize_t bytes_received = recv(ident, buf, sizeof(buf), 0);
    if (bytes_received <= 0) {
      if (bytes_received < 0)
        std::cerr << "Failed to receive data from client" << std::endl;
      DisconnectClient(ident);
      return 1;
    } else {
      buf[bytes_received] = '\0';
      clients_[ident] += buf;
      std::map<int, std::string>::iterator it = clients_.find(ident);
      if (it != clients_.end()) {
        if (clients_[ident] != "") {
          Response response(buf);
          response.ResponseHandler();
          std::string response_str = response.GetResponse();
          send(ident, response_str.c_str(), response_str.size(), 0);
          return 1;
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
