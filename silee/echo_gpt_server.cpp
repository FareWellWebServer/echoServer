#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int kBufferSize = 1024;

int main(int argc, char** argv) {

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
    return 1;
  }

  // Create a socket
  // 주소체계 : AF_INET
  // 사용 프로토콜 : SOCK_STREAM;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Set up the server address
  //sockaddr_in : INET4 관련 데이터가 있는 구조체
  sockaddr_in serv_addr;
  std::memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(std::stoi(argv[1]));

  // Bind the socket to the given port (ip주소와 port번호 부여.)
  sockaddr* addr_ptr = reinterpret_cast<sockaddr*>(&serv_addr);
  if (bind(sockfd, addr_ptr, sizeof(serv_addr)) < 0) {
    std::cerr << "Error binding socket to port" << std::endl;
    return 1;
  }

  // Start listening for connections
  // listen 이후 client에서 connect 함수 호출 가능
  if (listen(sockfd, 5) < 0) {
    std::cerr << "Error listening for connections" << std::endl;
    return 1;
  }

  std::cout << "Echo server is listening on port " << argv[1] << std::endl;

  // Accept incoming connections
    sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    sockaddr* cli_addr_ptr = reinterpret_cast<sockaddr*>(&cli_addr);

  while (true) {
    int newsockfd = accept(sockfd, cli_addr_ptr, &cli_len);
    
    if (newsockfd < 0) {
      std::cerr << "Error accepting connection" << std::endl;
      return 1;
    }

    char buffer[kBufferSize];
    std::memset(buffer, 0, kBufferSize);

    // Read data from the client
    ssize_t bytes_received = read(newsockfd, buffer, kBufferSize - 1);
    if (bytes_received < 0) {
      std::cerr << "Error reading data from client" << std::endl;
      return 1;
    }
    std::cout <<"recived : " << buffer << std::endl;

    // Echo the data back to the client
    ssize_t bytes_sent = write(newsockfd, buffer, bytes_received);
    if (bytes_sent < 0) {
      std::cerr << "Error sending data to client" << std::endl;
      return 1;
    }
    // Close the connection
    close(newsockfd);
  }

  return 0;
}
