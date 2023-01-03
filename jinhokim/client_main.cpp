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
    std::string fdStr(av[1]);

    for (char const& c: fdStr) {
        if (!std::isdigit(c)) {
            std::cerr << "Argument error: fd is not number" << std::endl;
            return false;
        }
    }

    // 올바른 fd인지 확인
    int fd = atoi(fdStr.c_str());

    if (fd < 3) {
        std::cerr << "Argument error: Wrong fd number" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief 
 * 
 * @param fd 
 */
void    main_process(int fd) {
    Client client(fd);

    std::cout << "Client fd: " << client.getFd() << std::endl;

    return ;
}

int main(int ac, char** av) {
    try {
        if (!checkArgument(ac, av))
            return EXIT_FAILURE;

        main_process(atoi(av[1]));

        return EXIT_SUCCESS;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}