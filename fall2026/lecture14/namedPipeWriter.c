#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(){
  // mkfifo creates the FIFO if it doesn't exist; harmless if it does
  mkfifo("/tmp/myFifo", 0666);

  printf("waiting for a reader on /tmp/myFifo...\n");
  int fd = open("/tmp/myFifo", O_WRONLY);
  printf("reader connected, sending messages\n");

  for(int i = 0; i < 5; i++){
    char msg[64];
    int n = snprintf(msg, sizeof(msg), "message %d down the pipe\n", i);
    write(fd, msg, n);
    sleep(1);
  }

  close(fd);
  return 0;
}
