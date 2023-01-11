#include "my_server.hpp"

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	my_server server(argv);
	char buffer[BUFFER_SIZE];
	try {
		server.Make_Socket();
		server.Do_Bind();
		server.Do_listen();
		std::cout << "Listening for incoming connections on port : "
				<< argv[1] << std::endl;
		while (1) {
			int	c_sock = accept(server.Get_sock(),
				 reinterpret_cast<sockaddr*>(&server.client_address_),
					&(server.client_address_size_));
			if (c_sock < 0) {
				std::cout << std::strerror(errno) << std::endl;
				throw my_server::Client_Socket_Create_Failed();
			}
			while (1) {
				server.Recv_n_Send(c_sock, buffer);
			}
			close(c_sock);
		}
	}
	catch(std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}
