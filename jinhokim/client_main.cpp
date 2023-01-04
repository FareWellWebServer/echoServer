#include "include/Client.hpp"

 // 성공 시 true, 실패 시 false 반환
int checkArgument(int ac, char **av) {
    // argument 개수 확인
    if (ac < 3) {
        std::cerr << "Few argument error" << std::endl;
        return false;
    }

    // hostname이 있는지 확인
    std::string hostStr(av[1]);

    if (hostStr.length() == 0) {
        std::cerr << "Invalid hostname" << std::endl;
        return false;
    }

    // fd가 숫자인지 확인
    std::string portStr(av[2]);

    for (std::size_t i = 0; i < portStr.size(); i++) {
        if (!std::isdigit(portStr[i])) {
            std::cerr << "Port is not number" << std::endl;
            return false;
        }
    }

    return true;
}

// 클라이언트 실행
void    main_process(std::string hostname, int port) {
    // Client client(port);

    // socket 생성, fd 얻음
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return ;
    }

    std::cout << "client_fd: " << client_fd << std::endl;

    return ;
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

// int main()
// {
//     int client_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (client_fd < 0)
//     {
//         std::cerr << "Failed to create socket" << std::endl;
//         return 1;
//     }

//     hostent* server = gethostbyname("localhost");
//     if (!server)
//     {
//         std::cerr << "Failed to resolve hostname" << std::endl;
//         return 1;
//     }

//     sockaddr_in server_address;
//     server_address.sin_family = AF_INET;
//     server_address.sin_port = htons(PORT);
//     std::memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);

//     int connect_result = connect(client_fd, (sockaddr*)&server_address, sizeof(server_address));
//     if (connect_result < 0)
//     {
//         std::cerr << "Failed to connect to server" << std::endl;
//         return 1;
//     }

//     std::cout << "Connected to server" << std::endl;

//     while (true)
//     {
//         std::string message;
//         std::getline(std::cin, message);
//         if (std::cin.eof())
//         {
//             break;
//         }

//         ssize_t bytes_sent = send(client_fd, message.c_str(), message.size(), 0);
//         if (bytes_sent < 0)
//         {
//             std::cerr << "Failed to send data to server" << std::endl;
//             break;
//         }

//         char buffer[1024];
//         ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
//         if (bytes_received < 0)
//         {
//             std::cerr << "Failed to receive data from server" << std::endl;
//             break;
//         }
//         else if (bytes_received == 0)
//         {
//             std::cout << "Server disconnected" << std::endl;
//             break;
//         }

//         std::cout << "Received from server: " << std::string(buffer, bytes_received) << std::endl;
//     }

//     close(client_fd);

//     return 0;
// }
