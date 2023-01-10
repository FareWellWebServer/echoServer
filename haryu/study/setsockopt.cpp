#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **artv)
{
    int s;
    int bsize = 0;
    int rn;
    rn = sizeof(int);
    s = socket(AF_INET, SOCK_STREAM, 0);

    // 현재 전송 소켓 버퍼의 크기를 가져온다.
    getsockopt(s, SOL_SOCKET, SO_SNDBUF, &bsize, (socklen_t *)&rn);
    printf("Send buf size : %d\n", bsize);

    // 현재 버퍼 크기에 * 2를한다.
    bsize = bsize * 2;
    setsockopt(s, SOL_SOCKET, SO_SNDBUF, &bsize, (socklen_t)rn);

    // 현재 버퍼크기를 가져온다. 
    // 커널은 * 2만큼 버퍼크기를 설정하므로, 최초 크기의 4배만큼 설정됐을 것이다.
    getsockopt(s, SOL_SOCKET, SO_SNDBUF, &bsize, (socklen_t *)&rn);
    printf("Send buf size : %d\n", bsize);
}