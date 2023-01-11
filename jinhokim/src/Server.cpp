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

#include "../include/Server.hpp"

Server::Server(int port) : port_(port) {}

Server::~Server(void) {}

std::string	Server::GetIp(void) {
	std::string	ip = std::string(inet_ntoa(address_.sin_addr));
	std::string	port = std::to_string(ntohs(address_.sin_port));
	return ip + ":" + port;
}

void    Server::ChangeEvents(std::vector<struct kevent>& change_list, int socket, int16_t filter,
                                uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
    struct kevent temp_event;

    EV_SET(&temp_event, socket, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

void    Server::DisconnectClient(int client_fd, std::map<int, std::string>& clients) {
    std::cerr << "Client disconnected" << std::endl;
    close(client_fd);
    clients.erase(client_fd);
}

void    Server::SetResponse(void) {
    if (!request_.compare("GET"))
        response_ = "GET!";
    else if (!request_.compare("POST"))
        response_ = "POST!!";
    else if (!request_.compare("DELETE"))
        response_ = "DELETE!!!";
    else
        response_ = "?????";

    return ;
}

int Server::Set(void) {
    // 소켓 생성
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
        return (PrintError("Failed to create socket"));

    address_.sin_family = AF_INET;
    address_.sin_port = htons(port_);
    address_.sin_addr.s_addr = inet_addr("127.0.0.1");

    int optval = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 소켓에 로컬 포트 번호 할당 
    int bind_result = bind(server_fd_, (sockaddr*)&address_, sizeof(address_));
    if (bind_result < 0)
        return (PrintError("Failed to bind socket to address"));

    // 커넥션을 받기 위해 로컬 소켓에 허용함을 표시
    int listen_result = listen(server_fd_, BACKLOG);
    if (listen_result < 0)
        return (PrintError("Failed to listen on socket"));

    return 0;
}

int Server::Run(void) {
    // init kqueue
    int kq = kqueue();
    if (kq == -1)
        return (PrintError("Failed kqueue init"));

    std::map<int, std::string> clients;
    std::vector<struct kevent> change_list;
    struct kevent event_list[128];

    // add event for server socket
    ChangeEvents(change_list, server_fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    int new_events = 1;
    struct kevent* curr_event;

    while (42) {
        // apply changes and return new events(pending events
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
        if (new_events == -1)
            return (PrintError("Failed kevent()"));

        change_list.clear();

        for (int i = 0; i < new_events; ++i) {
            curr_event = &event_list[i];

            // check event error
            if (curr_event->flags & EV_ERROR) {
                if (curr_event->ident == static_cast<uintptr_t>(server_fd_))
                    return (PrintError("Server socket error"));
                else
                    DisconnectClient(curr_event->ident, clients);
            }
            else if (curr_event->filter == EVFILT_READ) {
                if (curr_event->ident == static_cast<uintptr_t>(server_fd_)) {
                    // 클라이언트의 커낵션을 기다림
                    socklen_t client_len = sizeof(address_);
                    int client_fd = accept(server_fd_, (sockaddr*)&address_, &client_len);
                    if (client_fd == -1)
                        PrintError("Failed to accept incoming connection");
                    // 소켓 nonblocking 설정 
                    fcntl(client_fd, F_SETFL, O_NONBLOCK);

                    std::cout << "Accepted connection from " << GetIp() << std::endl;

                    // 클라이언트 소켓 이벤트 추가(recv, send)
                    ChangeEvents(change_list, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    ChangeEvents(change_list, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[client_fd] = "";
                }
                else if (clients.find(curr_event->ident) !=  clients.end()) {
					http_handler(curr_event->ident);
                    // // request 받기
                    // char buf[1024];
                    // ssize_t bytes_received = recv(curr_event->ident, buf, sizeof(buf), 0);
                    // if (bytes_received <= 0) { 
                    //     if (bytes_received < 0)
                    //         std::cerr << "Failed to receive data from client" << std::endl;
                    //     DisconnectClient(curr_event->ident, clients);
                    // }
                    // else {
                    //     buf[bytes_received] = '\0';
                    //     clients[curr_event->ident] += buf;
                    //     request_ = std::string(buf, bytes_received);
                    //     std::cout << "request from " << GetIp() << ": " << request_ << std::endl;

                    // 	// response 보내기
					// 	SetResponse();
					// 	std::map<int, std::string>::iterator it = clients.find(curr_event->ident);
					// 	if (it != clients.end()) {
					// 		if (clients[curr_event->ident] != "") {
					// 			ssize_t bytes_sent = send(curr_event->ident, response_.c_str(), response_.size(), 0);
					// 			if (bytes_sent < 0)
					// 				std::cerr << "Failed to send data to client" << std::endl;
					// 			else
					// 				clients[curr_event->ident].clear();
					// 		}
					// 	}
					// }
				}
                // else if (curr_event->filter == EVFILT_WRITE) {
                //     // response 보내기
                //     SetResponse();
                //     std::cout << "response: " << response_ << std::endl;
                //     std::map<int, std::string>::iterator it = clients.find(curr_event->ident);
                //     if (it != clients.end()) {
                //         if (clients[curr_event->ident] != "") {
                //             ssize_t bytes_sent = send(curr_event->ident, response_.c_str(), response_.size(), 0);
                //             if (bytes_sent < 0)
                //                 std::cerr << "Failed to send data to client" << std::endl;
                //             else
                //                 clients[curr_event->ident].clear();
                //         }
                //     }
                // }
            }
        }
    }
    return 0;
}

int PrintError(const std::string str) {
    std::cerr << str << std::endl;
    return 1;
}	

void fill_header(char *header, int status, long len, std::string type) {
    char status_text[40];
    switch (status) {
        case 200:
            strcpy(status_text, "OK"); break;
        case 404:
            strcpy(status_text, "Not Found"); break;
        case 500:
        default:
            strcpy(status_text, "Internal Server Error"); break;
    }
    sprintf(header, HEADER_FORMAT, status, status_text, len, type.c_str());
}

void handle_404(int asock) {
    char header[BUFSIZE];
	std::string t("text/html");
    fill_header(header, 404, sizeof(NOT_FOUND_CONTENT), t);

    write(asock, header, strlen(header));
    write(asock, NOT_FOUND_CONTENT, sizeof(NOT_FOUND_CONTENT));
}

void handle_500(int asock) {
    char header[1024];
	std::string t("text/html");
    fill_header(header, 500, sizeof(SERVER_ERROR_CONTENT), t);

    write(asock, header, strlen(header));
    write(asock, SERVER_ERROR_CONTENT, sizeof(SERVER_ERROR_CONTENT));
}

void find_mime(char *ct_type, char *uri) {
    char *ext = strrchr(uri, '.');
    if (!strcmp(ext, ".html"))
        strcpy(ct_type, "text/html");
    else if (!strcmp(ext, ".jpg") || !strcmp(ext, ".jpeg"))
        strcpy(ct_type, "image/jpeg");
    else if (!strcmp(ext, ".png"))
        strcpy(ct_type, "image/png");
    else if (!strcmp(ext, ".css"))
        strcpy(ct_type, "text/css");
    else if (!strcmp(ext, ".js"))
        strcpy(ct_type, "text/javascript");
    else strcpy(ct_type, "text/plain");
}

void http_handler(int asock) {
    char header[BUFSIZE];
    char buf[BUFSIZE];

    if (read(asock, buf, BUFSIZE) < 0) {
        perror("[ERR] Failed to read request.\n");
        handle_500(asock); return;
    }

    char *method = strtok(buf, " ");
    char *uri = strtok(NULL, " ");
    if (method == NULL || uri == NULL) {
        perror("[ERR] Failed to identify method, URI.\n");
        handle_500(asock); return;
    }

    printf("[INFO] Handling Request: method=%s, URI=%s\n", method, uri);

    char safe_uri[BUFSIZE];
    char *local_uri;
    struct stat st;

    strcpy(safe_uri, uri);
    if (!strcmp(safe_uri, "/"))
		strcpy(safe_uri, "/index.html");

    local_uri = safe_uri + 1;
    if (stat(local_uri, &st) < 0) {
        perror("[WARN] No file found matching URI.\n");
        handle_404(asock); return;
    }

    int fd = open(local_uri, O_RDONLY);
    if (fd < 0) {
        perror("[ERR] Failed to open file.\n");
        handle_500(asock); return;
    }

    int ct_len = st.st_size;
    char ct_type[40];
    find_mime(ct_type, local_uri);
    fill_header(header, 200, ct_len, ct_type);
    write(asock, header, strlen(header));

    int cnt;
    while ((cnt = read(fd, buf, BUFSIZE)) > 0)
        write(asock, buf, cnt);
}
