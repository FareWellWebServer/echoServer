#include "include/Client.hpp"

bool    printError(const std::string str) {
    std::cerr << str << std::endl;
    return false;
}

 // 성공 시 true, 실패 시 false 반환
bool	checkArgument(int ac, char **av) {
    // argument 개수 확인
    if (ac < 3)
        return (printError("Few argument error"));

    // hostname이 있는지 확인
    std::string hostStr(av[1]);
    if (hostStr.length() == 0)
        return (printError("Invalid hostname"));

    // fd가 숫자인지 확인
    std::string portStr(av[2]);
    for (std::size_t i = 0; i < portStr.size(); i++) {
        if (!std::isdigit(portStr[i]))
            return (printError("Port is not number"));
    }

    return true;
}

// 클라이언트 실행
int	main_process(std::string hostname, int port) {
	Client client(hostname, port);
    std::cout << "hostname: " << client.getHostname() << std::endl;
    std::cout << "port: " << client.getPort() << std::endl;

	/**
	 * socket 생성, fd 얻음
	 * AF_INET: specifies that the socket will use the IPv4 protocol, 
	 * SOCK_STREAM: specifies that the socket will use a stream-based protocol (in this case, TCP)
	 */
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
        return (printError("Failed to create socket"));
    std::cout << "client_fd: " << client_fd << std::endl;

	// hostname(도메인 주소)를 ip 주소로 변환
    hostent* server = gethostbyname(hostname.c_str());
    if (!server)
        return (printError("Failed to resolve hostname"));
    std::cout << "server name: " << server->h_name << std::endl;

	/**
	 * sockaddr_in: used to store IPv4 addresses
	 * sin_family: specifies that the address is an IPv4 address
	 * sin_port: specifies the port number
	 * sin_addr: specifies the IP address of the server
	 * h_addr: used to set the IP address
	 * h_length: specifies the length of the h_addr
	 */
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    std::memcpy(&server_address.sin_addr.s_addr, server->h_addr, \
					server->h_length);

	/**
	 * 클라이언트와 서버 연결
	 * TODO -> 서버 구현
	 * connect func: used by a client to establish a connection to a server.
	 */
    int connect_result = connect(client_fd, (sockaddr*)&server_address, \
                                    sizeof(server_address));
    if (connect_result < 0)
        return (printError("Failed to connect to server"));

	std::cout << "Connected to server" << std::endl;

	/**
	 * message 읽어서 서버와 통신해보기
	 */
    while (42) {
        std::string message;
        std::getline(std::cin, message);
        if (std::cin.eof())
            break ;

        ssize_t bytes_sent = send(client_fd, message.c_str(), message.size(), 0);
        if (bytes_sent < 0)
			return (printError("Failed to send data to server"));

        char buffer[2048];
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received < 0)
			return (printError("Failed to eceive data from server"));
        else if (bytes_received == 0)
			return (printError("Server disconnected"));

        std::cout << "Received from server: " << std::string(buffer, bytes_received) << std::endl;
    }

    close(client_fd);

    return true;
}

int main(int ac, char** av) {
    try {
        if (!checkArgument(ac, av))
            return EXIT_FAILURE;

        main_process(av[1], atoi(av[2]));

        return EXIT_SUCCESS;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
