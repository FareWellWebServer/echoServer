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
    if (ac < 2) {
        std::cerr << "Argument error: few argument" << std::endl;
        return false;
    }

    // port가 숫자인지 확인
    std::string portStr(av[1]);

    for (char const& c: portStr) {
        if (!std::isdigit(c)) {
            std::cerr << "Argument error: port is not number" << std::endl;
            return false;
        }
    }

    // 올바른 port인지 확인
    int port = atoi(portStr.c_str());

    if (port < 3) {
        std::cerr << "Argument error: Wrong port number" << std::endl;
        return false;
    }

    return true;
}

// int main_process(int port) {
//     std::cout << port << std::endl;
//     return EXIT_SUCCESS;
// }

int main(int ac, char** av) {
    try {
        if (!checkArgument(ac, av))
            return EXIT_FAILURE;

        //main_process(atoi(port.c_str()));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}