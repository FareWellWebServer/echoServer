#include "my_server.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	
	my_server server(argv);
	
	try
	{
		server.Make_Socket();
		server.Do_Bind();
		std::cout << "Listening for incoming connections on port : " << argv[1] << std::endl;
		while (1) {
			int	c_sock = accept(server.Get_sock(), reinterpret_cast<sockaddr*>(&server.client_address_),
				&(server.client_address_size_));
			if (c_sock < 0)
				throw my_server::Client_Socket_Create_Failed();
			char buffer[BUFFER_SIZE];
			


		}	

	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

}