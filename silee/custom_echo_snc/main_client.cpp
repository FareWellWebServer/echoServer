#include "my_client.hpp"

int main(int argc, char **argv) {
    if (argc != 3)
        return (1);
    try {
        my_client client(argv);
        client.Make_Socket();
        client.Do_Connect();
        while (1)
            client.Send_Msg();
        client.Close_Socket();
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
