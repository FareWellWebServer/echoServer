/*
socket() -> 연결되지 않은 새로운 소켓 생성
bind() -> socket에 로컬 포트 번호 할당 
listen() -> 커넥션을 받기 위해 로컬 소켓에 허용함을 표시
accept() -> 누군가 로컬 포트에 커낵션을 맺기를 기다림
recv() -> request 받음
send() -> response 보냄
close() -> 소켓 닫음

EX)
1. 새로운 socket 생성(socket)
2. 80 포트로 socket을 묶는다.
3. socket 커낵션을 허가함(listen)
4. 커낵션을 기다림(accept)

- 한 클라이언트와 커낵션됨 -
5. 요청(http request)을 기다림(recv)

- request 받음 -
6. request 처리
7. response 보냄(send)
8. 커낵션을 닫는다(close)
*/

#include "../include/Server.hpp"

Server::Server(int port) : port_(port) {}

Server::~Server(void) {
    close(client_fd_);
}

void    Server::setResponse(void) {
    if (!request_.compare("GET"))
        response_ = "request=GET";
    else if (!request_.compare("POST"))
        response_ = "request=POST";
    else if (!request_.compare("DELETE"))
        response_ = "request=DELETE";
    else
        response_ = "request=??";

    return ;
}

int Server::set(void) {
    // 소켓 생성
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
        return (printError("Failed to create socket"));

    address_.sin_family = AF_INET;
    address_.sin_port = htons(port_);
    address_.sin_addr.s_addr = INADDR_ANY;

    // 소켓에 로컬 포트 번호 할당 
    int bind_result = bind(server_fd_, (sockaddr*)&address_, sizeof(address_));
    if (bind_result < 0)
        return (printError("Failed to bind socket to address"));

    // 커넥션을 받기 위해 로컬 소켓에 허용함을 표시
    int listen_result = listen(server_fd_, BACKLOG);
    if (listen_result < 0)
        return (printError("Failed to listen on socket"));

    return 0;
}

int Server::run(void) {
    while (42) {
        // 클라이언트의 커낵션을 기다림
        socklen_t client_len = sizeof(address_);
        int client_fd_ = accept(server_fd_, (sockaddr*)&address_, &client_len);
        if (client_fd_ < 0) {
            printError("Failed to accept incoming connection");
            continue;
        }

        std::cout << "Accepted connection from "
                    << inet_ntoa(address_.sin_addr)
                    << ":"
                    << ntohs(address_.sin_port)
                    << std::endl;

        while (42) {
            // request 받기
            char buffer[1024];
            ssize_t bytes_received = recv(client_fd_, buffer, sizeof(buffer), 0);
            if (bytes_received < 0)
                return (printError("Failed to receive data from client"));
            else if (bytes_received == 0)
                return (printError("Client disconnected"));

            request_ = std::string(buffer, bytes_received);
            std::cout << "request: " << request_ << std::endl;

            // response 보내기
            setResponse();
            std::cout << "response: " << response_ << std::endl;

            ssize_t bytes_sent = send(client_fd_, response_.c_str(), response_.size(), 0);
            if (bytes_sent < 0)
                return (printError("Failed to send data to client"));
        }
    }
    return 0;
}

int printError(const std::string str) {
    std::cerr << str << std::endl;
    return 1;
}	
