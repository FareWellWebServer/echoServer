#include "../include/Server.hpp"

Server::Server(void) {
    ;
}

Server::~Server(void) {
    close(_client_fd);
}
/**
 * @brief 
 * socket 생성 -> fd 얻음  
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int Server::createSocket(void) {
    // AF_INET: specifies socket will use the IPv4 protocol, 
    // SOCK_STREAM: specifies socket will use a stream-based protocol(in this case, TCP)
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
        return 1;
    return 0;
}

/**
 * @brief 
 * socket에 ip 주소를 할당
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int Server::bindSocket(void) {
	/**
	 * sockaddr_in: used to store IPv4 addresses
	 * sin_family: specifies address is an IPv4 address
	 * sin_port: specifies port number
	 * sin_addr: specifies IP address of the server
     * INADDR_ANY: specify a socket should listen on all available network interfaces
	 */
    _address.sin_family = AF_INET;
    _address.sin_port = htons(PORT);
    _address.sin_addr.s_addr = INADDR_ANY;

    int bind_result = bind(_server_fd, (sockaddr*)&_address, sizeof(_address));
    if (bind_result < 0)
        return 1;
    return 0;
}

/**
 * @brief 
 * server setting
 * @return int 
 * 성공 시 0, 실패 시 1 반환
 */
int Server::set(void) {
    if (createSocket())
        return (printError("Failed to create socket"));

    if (bindSocket())
        return (printError("Failed to bind socket to address"));

    int listen_result = listen(_server_fd, BACKLOG);
    if (listen_result < 0)
        return (printError("Failed to listen on socket"));

    return 0;
}

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

        std::cout << "request wait" << std::endl;

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
