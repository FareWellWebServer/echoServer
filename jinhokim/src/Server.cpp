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

Server::Server(int port) : _port(port) {}

Server::~Server(void) {
    close(_client_fd);
}

/**
 * @brief 
 * request에 맞게 response 매칭
 */
void    Server::setResponse(void) {
    if (!_request.compare("GET"))
        _response = "request=GET";
    else if (!_request.compare("POST"))
        _response = "request=POST";
    else if (!_request.compare("DELETE"))
        _response = "request=DELETE";
    else
        _response = "request=??";

    return ;
}

/**
 * @brief 
 * server setting
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int Server::set(void) {
   // AF_INET: specifies socket will use the IPv4 protocol, 
    // SOCK_STREAM: specifies socket will use a stream-based protocol(in this case, TCP)
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
        return (printError("Failed to create socket"));

	/**
	 * sockaddr_in: used to store IPv4 addresses
	 * sin_family: specifies address is an IPv4 address
	 * sin_port: specifies port number
	 * sin_addr: specifies IP address of the server
     * INADDR_ANY: specify a socket should listen on all available network interfaces
	 */
    _address.sin_family = AF_INET;
    _address.sin_port = htons(_port);
    _address.sin_addr.s_addr = INADDR_ANY;

    int bind_result = bind(_server_fd, (sockaddr*)&_address, sizeof(_address));
    if (bind_result < 0)
        return (printError("Failed to bind socket to address"));

    int listen_result = listen(_server_fd, BACKLOG);
    if (listen_result < 0)
        return (printError("Failed to listen on socket"));

    return 0;
}

/**
 * @brief 
 * 클라이언트와 통신 시작
 * @return int 
 * 정상 종료시 0, 실패 시 1 반환
 */
int Server::run(void) {
    while (42) {
        socklen_t client_len = sizeof(_client_address);
        int _client_fd = accept(_server_fd, (sockaddr*)&_client_address, &client_len);
        if (_client_fd < 0) {
            printError("Failed to accept incoming connection");
            continue;
        }

        std::cout << "Accepted connection from "
                    << inet_ntoa(_client_address.sin_addr)
                    << ":"
                    << ntohs(_client_address.sin_port)
                    << std::endl;

        while (42) {
            char buffer[1024];
            ssize_t bytes_received = recv(_client_fd, buffer, sizeof(buffer), 0);
            if (bytes_received < 0)
                return (printError("Failed to receive data from client"));
            else if (bytes_received == 0)
                return (printError("Client disconnected"));

            _request = std::string(buffer, bytes_received);
            std::cout << "request: " << _request << std::endl;

            setResponse();
            std::cout << "response: " << _response << std::endl;

            ssize_t bytes_sent = send(_client_fd, _response.c_str(), _response.size(), 0);
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

/**
 * @brief 
 * argument 에러 처리
 * @param ac 
 * @param av 
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int checkArgument(int ac, char **av) {
    // argument 개수 확인
    if (ac < 2)
        return (printError("Few argument error"));

    // port가 숫자인지 확인
    std::string portStr(av[1]);
    for (std::size_t i = 0; i < portStr.size(); i++) {
        if (!std::isdigit(portStr[i]))
            return (printError("Port is not number"));
    }

    return 0;
}
