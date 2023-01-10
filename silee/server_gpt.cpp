#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const int BUFFER_SIZE = 1024;

int main(int argc, char* argv[])
{
	// Check the number of arguments
	if (argc != 2) {
		std::cerr << "Usage: echo_server <port>" << std::endl;
		return 1;
	}

	// Create a socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cerr << "Failed to create a socket" << std::endl;
		return 1;
	}

	// Bind the socket to a port
	sockaddr_in server_address;
	
	std::memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	// server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = htons(std::atoi(argv[1]));

	// Bind the socket to the given port (ip주소와 port번호 부여.)
	//bind 함수의 두번 째 인자는 sockaddr 구조체를 받음으로 casting해줘야함. 근데 왜 reinterpret..?
	if (bind(sock, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
		std::cerr << "Failed to bind the socket to a port" << std::endl;
		return 1;
	}

	// Start listening for incoming connections
	if (listen(sock, 5) < 0) {
		std::cerr << "Failed to start listening for incoming connections" << std::endl;
		return 1;
	}

	std::cout << "Listening for incoming connections on port " << argv[1] << "..." << std::endl;

	// Accept incoming connections
	while (true) {

		sockaddr_in client_address;
		socklen_t client_address_size = sizeof(client_address);

		//accept의 두번째 인자 : 대기 큐를 참조해 얻은 client의 주소
		int client_sock = accept(sock, reinterpret_cast<sockaddr*>(&client_address), 
			&client_address_size);

		if (client_sock < 0) {
			std::cerr << "Failed to accept incoming connection" << std::endl;
			continue;
		}

		std::cout << "Accepted incoming connection from " 
			<< inet_ntoa(client_address.sin_addr) << ":" << 
				ntohs(client_address.sin_port) << std::endl;

		// Receive messages from the client and send them back
		char buffer[BUFFER_SIZE];
		while (true) {
			std::memset(buffer, 0, BUFFER_SIZE);
			int bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
			if (bytes_received == 0) {
				std::cout << "Client disconnected" << std::endl;
				break;
			}

			std::cout << "Received message from client: " << buffer << std::endl;

			int bytes_sent = send(client_sock, buffer, bytes_received, 0);
			if (bytes_sent < 0) {
				std::cerr << "Failed to send message to client" << std::endl;
				break;
			}
		}

		// Close the client socket
		close(client_sock);
	}

	// Close the server socket
	close(sock);

	return 0;
}
