#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
  int fd = open("/dev/urandom", O_RDONLY);

  unsigned char buf[16];
  read(fd, buf, sizeof(buf));
  close(fd);

  printf("sixteen random bytes from the kernel: ");
  for(int i = 0; i < 16; i++){
    printf("%02x", buf[i]);
  }
  printf("\n");

  return 0;
}
