// this is the server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

int fdRead, fdWrite;

void cleaner(int sig){
  printf("\n Cleaning up! Goodbye!\n");
  close(fdRead);
  close(fdWrite);
  unlink("serverToClient");
  unlink("clientToServer");
  exit(0);
}

int main(){
  const int bSize = 1024;
  char buffer[bSize];

  mkfifo("clientToServer", 0666);
  mkfifo("serverToClient", 0666);

  printf("Server started. Waiting for yapping...\n");
  fdWrite = open("serverToClient", O_WRONLY);
  fdRead = open("clientToServer", O_RDONLY);

  signal(SIGINT, cleaner);

  while(1){
    // clear the buffer
    memset(buffer, 0, bSize);
    int bytesRead = read(fdRead, buffer, bSize);
    if(bytesRead > 0){
      printf("Client: %s\n", buffer);
      write(fdWrite, buffer, bytesRead);
     }
    else if(bytesRead == 0){
      close(fdRead);
      fdRead = open("clientToServer", O_RDONLY);
      if(fdRead < 0){
        break;
      }
    }
    else {
      break;
    }
  }

  close(fdRead);
  close(fdWrite);
  unlink("clientToServer");
  unlink("serverToClient");
}
