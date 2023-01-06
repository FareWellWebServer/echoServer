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

Client::Client(std::string hostname, int port) : 
    _hostname(hostname), 
    _port(port) {
    std::cout << "hostname: " << _hostname << std::endl;
    std::cout << "port: " << _port << std::endl;
}

Client::~Client(void) {
    close(_client_fd);
}

/**
 * @brief 
 * socket 생성 -> fd 얻음  
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int Client::createSocket(void) {
    // AF_INET: specifies that the socket will use the IPv4 protocol, 
    // SOCK_STREAM: specifies that the socket will use a stream-based protocol (in this case, TCP)
    _client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_client_fd < 0)
        return 1;
    return 0;
}

/**
 * @brief 
 * 서버 세팅: hostname(도메인 주소)를 ip 주소로 변환
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int Client::setServer(void) {
    _server = gethostbyname(_hostname.c_str());
    if (!_server)
        return 1;
	/**
	 * sockaddr_in: used to store IPv4 addresses
	 * sin_family: specifies that the address is an IPv4 address
	 * sin_port: specifies the port number
	 * sin_addr: specifies the IP address of the server
	 * h_addr: used to set the IP address
	 * h_length: specifies the length of the h_addr
	 */
    _server_address.sin_family = AF_INET;
    _server_address.sin_port = htons(_port);
    std::memcpy(&_server_address.sin_addr.s_addr, _server->h_addr, \
					_server->h_length);
    return 0;
}

/**
 * @brief 
 * 클라이언트와 서버 연결
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int Client::connectServer(void) {
    // connect(): used by a client to establish a connection to a server
    int connect_result = connect(_client_fd, (sockaddr*)&_server_address, \
                                    sizeof(_server_address));
    if (connect_result < 0)
        return 1;
    return 0;
}

/**
 * @brief 
 * 서버에 메시지 전송
 * @param message 
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int Client::sendRequest(std::string message) const {
    // send func: used to send the message to the server
    ssize_t bytes_sent = send(_client_fd, message.c_str(), message.size(), 0);
    if (bytes_sent < 0)
        return 1;
    return 0;
}

/**
 * @brief 
 * client setting
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int    Client::set(void) {
    if (createSocket())
        return (printError("Failed to create socket"));

    if (setServer())
        return (printError("Failed to resolve hostname"));

    if (connectServer())
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
    while (42) {
        std::string message;
        std::getline(std::cin, message);
        if (std::cin.eof() || !message.compare("exit")) {
            std::cout << "Client Bye!" << std::endl;
            break ;
        }

        if (sendRequest(message))
			return (printError("Failed to send data to server"));

        char buffer[1024];

        // recv(): used to receive the response from the server
        ssize_t bytes_received = recv(_client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received < 0)
			return (printError("Failed to receive data from server"));
        else if (bytes_received == 0)
			return (printError("Server disconnected"));

        _response = std::string(buffer, bytes_received);
        std::cout << "Received from server: " << _response << std::endl;
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
    if (ac < 3)
        return (printError("Few argument error"));

    // hostname이 있는지 확인
    std::string hostStr(av[1]);
    if (hostStr.length() == 0)
        return (printError("Invalid hostname"));

    // port가 숫자인지 확인
    std::string portStr(av[2]);
    for (std::size_t i = 0; i < portStr.size(); i++) {
        if (!std::isdigit(portStr[i]))
            return (printError("Port is not number"));
    }

    return 0;
}
