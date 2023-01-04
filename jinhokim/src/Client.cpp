#include "../include/Client.hpp"

Client::Client(std::string hostname, int port) : 
    _hostname(hostname), 
    _port(port) {};

Client::~Client(void) {
    close(_clientFd);
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
    _clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_clientFd < 0)
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
    _server_address.sin_port = htons(PORT);
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
    int connect_result = connect(_clientFd, (sockaddr*)&_server_address, \
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
int Client::sendMessage(std::string message) const {
    // send func: used to send the message to the server
    ssize_t bytes_sent = send(_clientFd, message.c_str(), message.size(), 0);
    if (bytes_sent < 0)
        return 1;
    return 0;
}

/**
 * @brief 
 * 서버로부터 메시지 받기
 * @return int 
 * 서버로부터 받은 메시지의 바이트
 */
int Client::receiveMessage(void) {
    char buffer[2048];

    // recv(): used to receive the response from the server
    ssize_t bytes_received = recv(_clientFd, buffer, sizeof(buffer), 0);
    _response = std::string(buffer, bytes_received);
    return bytes_received;
}

const std::string Client::getHostname(void) const {
	return this->_hostname;
}

int Client::getPort(void) const {
	return this->_port;
}

int Client::getClientFd(void) const {
	return this->_clientFd;
}

const std::string Client::getResponse(void) const {
	return this->_response;
}
