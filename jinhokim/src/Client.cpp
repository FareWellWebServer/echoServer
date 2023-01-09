/*
socket() -> 연결되지 않은 새로운 소켓 생성
connect() -> 로컬의 소켓과 원격 호스트 및 포트 사이에 TCP 커낵션 생성
send() -> request 보냄
recv() -> response 받음
close() -> 소켓 닫음

EX)
1. ip 주소와 포트를 얻는다.
2. 새로운 socket 생성(socket)
3. 서버의 ip 포트로 연결(connect)

- 서버와 연결 성공 -

4. 요쳥(http request) 보냄(send)
5. 응답(http response) 기다림(recv)

- 서버로 부터 response를 받음 -

6. http response를 처리한다.
7. 커낵션을 닫는다(close)
*/

#include "../include/Client.hpp"

Client::Client(int port) : port_(port) {}

Client::~Client(void) {
    close(client_fd_);
}

int    Client::set(void) {
    // 소켓 생성
    client_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd_ < 0)
        return (printError("Failed to create socket"));

    server_ = gethostbyname("localhost");
    if (!server_)
        return (printError("Failed to resolve hostname"));

    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(port_);
    std::memcpy(&server_address_.sin_addr.s_addr, server_->h_addr, \
					server_->h_length);

    // 서버와 연결
    int connect_result = connect(client_fd_, (sockaddr*)&server_address_, \
                                    sizeof(server_address_));
    if (connect_result < 0)
        return (printError("Failed to connect to server"));

	std::cout << "Connected to server" << std::endl;

    return 0;
}

int    Client::run(void) {
    char buffer[1024];

    while (42) {
        // 입력 받기
        std::string message;
        std::getline(std::cin, message);
        if (std::cin.eof() || !message.compare("exit")) {
            std::cout << "Client Bye!" << std::endl;
            break ;
        }

        // request 보내기
        ssize_t bytes_sent = send(client_fd_, message.c_str(), message.size(), 0);
        if (bytes_sent < 0)
			return (printError("Failed to send data to server"));

        // response 받기
        ssize_t bytes_received = recv(client_fd_, buffer, sizeof(buffer), 0);
        if (bytes_received < 0)
			return (printError("Failed to receive data from server"));
        else if (bytes_received == 0)
			return (printError("Server disconnected"));

        response_ = std::string(buffer, bytes_received);
        std::cout << "Received from server: " << response_ << std::endl;
    }
    return 0;
}

int printError(const std::string str) {
    std::cerr << str << std::endl;
    return 1;
}	
