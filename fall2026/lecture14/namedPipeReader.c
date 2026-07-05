// the matching reader for namedPipeWriter.c
// open the FIFO and loop reading until the writer closes its end
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
  printf("waiting for a writer on /tmp/myFifo...\n");
  int fd = open("/tmp/myFifo", O_RDONLY);
  printf("writer connected, reading messages\n");

  char buf[128];
  ssize_t n;
  while((n = read(fd, buf, sizeof(buf) - 1)) > 0){
    buf[n] = 0;
    printf("got: %s", buf);
  }
  close(fd);
  return 0;
}
