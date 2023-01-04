#include "include/Client.hpp"

int printError(const std::string str) {
    std::cerr << str << std::endl;
    return 1;
}

 // 성공 시 0, 실패 시 1 반환
int checkArgument(int ac, char **av) {
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

    return 0;
}

// 클라이언트 실행
int	main_process(std::string hostname, int port) {
	Client client(hostname, port);
    std::cout << "hostname: " << client.getHostname() << std::endl;
    std::cout << "port: " << client.getPort() << std::endl;

    if (client.createSocket())
        return (printError("Failed to create socket"));
    std::cout << "client fd: " << client.getClientFd() << std::endl;

    if (client.setServer())
        return (printError("Failed to resolve hostname"));

    if (client.connectServer())
        return (printError("Failed to connect to server"));
	std::cout << "Connected to server" << std::endl;

    // message 읽어서 서버와 통신해보기
    // while (42) {
    //     std::string message;
    //     std::getline(std::cin, message);
    //     if (std::cin.eof())
    //         break ;

    //     // send func: used to send the message to the server
    //     ssize_t bytes_sent = send(client_fd, message.c_str(), message.size(), 0);
    //     if (bytes_sent < 0)
	// 		return (printError("Failed to send data to server"));

    //     char buffer[2048];
    //     // recv func: used to receive the response from the server
    //     ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    //     if (bytes_received < 0)
	// 		return (printError("Failed to eceive data from server"));
    //     else if (bytes_received == 0)
	// 		return (printError("Server disconnected"));

    //     std::cout << "Received from server: " << std::string(buffer, bytes_received) << std::endl;
    // }

    return true;
}

int main(int ac, char** av) {
    try {
        if (checkArgument(ac, av))
            return EXIT_FAILURE;

        main_process(av[1], atoi(av[2]));

        return EXIT_SUCCESS;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
