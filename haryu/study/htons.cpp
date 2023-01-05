#include <sys/time.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
// #include <stdlib.h>
#include <cstdlib>
// #include <stdio.h>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    int client_sockfd;
    struct sockaddr_in clientaddr;
    int data = 123456789;
//    int client_len;
    socklen_t client_len; /* socklen_t == unsigned int */

    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = inet_addr("192.168.100.190");
    clientaddr.sin_port = htons(atoi(argv[1]));

    client_len = sizeof(clientaddr);

    //if (connect(client_sockfd, (struct sockaddr *)&clientaddr, client_len) < 0)
    if (connect(client_sockfd, reinterpret_cast<struct sockaddr *>(&clientaddr), client_len) < 0)
    {
        perror("Connect error : ");
        exit(0);
    }
    
    // 보낼 데이터 네트워크 byte order를 따르도록 변경한다. 
    data = htonl(data);
    write(client_sockfd, (void *)&data, sizeof(int));

    // 읽어들인 데이터는 호스트 byte order을 따르도록 변경한다.
    read(client_sockfd, (void *)&data, sizeof(int));
    data = ntohl(data);
    close(client_sockfd);
}
