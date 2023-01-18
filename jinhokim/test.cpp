#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

int main(void) {
  int fd = open("html/200.png", O_RDONLY);
  if (fd < 0) return write(2, "File Error", 10);

  off_t file_size = lseek(fd, 0, SEEK_END);
  if (file_size < 0) return write(2, "Lseek Error", 11);

  std::cout << "lseek file size: " << file_size << std::endl;

  char uri[13] = "html/200.png";
  struct stat st;
  stat(uri, &st);
  std::cout << "stat file size: " << st.st_size << std::endl;
}