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

/**
 * @brief 
 * client setting
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int    Client::set(void) {
    // AF_INET: specifies that the socket will use the IPv4 protocol, 
    // SOCK_STREAM: specifies that the socket will use a stream-based protocol (in this case, TCP)
    client_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd_ < 0)
        return (printError("Failed to create socket"));

    server_ = gethostbyname("localhost");
    if (!server_)
        return (printError("Failed to resolve hostname"));
	/**
	 * sockaddr_in: used to store IPv4 addresses
	 * sin_family: specifies that the address is an IPv4 address
	 * sin_port: specifies the port number
	 * sin_addr: specifies the IP address of the server
	 * h_addr: used to set the IP address
	 * h_length: specifies the length of the h_addr
	 */
    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(port_);
    std::memcpy(&server_address_.sin_addr.s_addr, server_->h_addr, \
					server_->h_length);

    // connect(): used by a client to establish a connection to a server
    int connect_result = connect(client_fd_, (sockaddr*)&server_address_, \
                                    sizeof(server_address_));
    if (connect_result < 0)
        return (printError("Failed to connect to server"));

	std::cout << "Connected to server" << std::endl;

    return 0;
}

/**
 * @brief 
 * 서버와 통신 시작
 * @return int 
 * 정상 종료시 0, 실패 시 1 반환
 */
int    Client::run(void) {
    char buffer[1024];

    while (42) {
        std::string message;
        std::getline(std::cin, message);
        if (std::cin.eof() || !message.compare("exit")) {
            std::cout << "Client Bye!" << std::endl;
            break ;
        }

        // send func: used to send the message to the server
        ssize_t bytes_sent = send(client_fd_, message.c_str(), message.size(), 0);
        if (bytes_sent < 0)
			return (printError("Failed to send data to server"));

        // recv(): used to receive the response from the server
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
