#include "include/Client.hpp"

/**
 * @brief 
 * 
 * @param ac 
 * @param av 
 * @return int 
 * 성공 시 true, 실패 시 false 반환
 */
int checkArgument(int ac, char **av) {
    // argument 개수 확인
    if (ac < 3) {
        std::cerr << "Argument error: few argument" << std::endl;
        return false;
    }

    // fd가 숫자인지 확인
    std::string portStr(av[2]);

    for (char const& c: portStr) {
        if (!std::isdigit(c)) {
            std::cerr << "Argument error: port is not number" << std::endl;
            return false;
        }
    }

    return true;
}

/**
 * @brief 
 * 
 * @param port 
 */
void    main_process(int port) {
    Client client(port);
    std::cout << "client fd: " << client.getPort() << std::endl;

    // socket 생성, fd 얻음
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0) {
        std::cerr << "Socket creat error: socket can't be created" << std::endl;
        return ;
    }
    std::cout << "sock_fd: " << sock_fd << std::endl;

    /*
    sockaddr_in server_addr;
    hostent     *server;

    server = gethostbyname(av[1]);
    */

    return ;
}

int main(int ac, char** av) {
    try {
        if (!checkArgument(ac, av))
            return EXIT_FAILURE;

        main_process(atoi(av[2]));

        return EXIT_SUCCESS;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}