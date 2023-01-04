// select_exam.c

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

int main(){
  int fd, n, ret;

  char buf[128];
  struct timeval tv;

  fd_set readfds;
  fd = open("select_file",O_RDONLY);
  if(fd < 0){
        perror("file open error ");
        exit(0);
  }
  memset(buf, 0x00, 128);
  
  FD_ZERO(&readfds); // readfds 를 초기화
  while(1){
        FD_SET(fd,&readfds); // 감시할 세트를 준비

        ret = select(fd+1, &readfds, NULL, NULL, NULL); // select로 감시를 시작

        if(ret == -1){ // 에러케이스
                perror("select error ");
                exit(0);
        }

        if(FD_ISSET(fd, &readfds)){ // flag가 반환되면 해당 매크로 검출
                while(( n = read(fd, buf, 128)) > 0) // 마지막까지 내용을 출력
                        printf("%s",buf);
        }

        memset(buf, 0x00, 128);
        usleep(1000);
  }
}