// this is the server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

int fdRead = -1, fdWrite = -1;
volatile sig_atomic_t stop_requested = 0;

void cleaner(int sig){
  static const char msg[] = "\n Cleaning up! Goodbye!\n";
  (void)sig;
  stop_requested = 1;
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void cleanup_resources(void){
  if(fdRead != -1) close(fdRead);
  if(fdWrite != -1) close(fdWrite);
  unlink("serverToClient");
  unlink("clientToServer");
}

int main(){
  struct sigaction sa = {0};
  printf("I am %d\n",getpid());
  
  const int bSize = 1024;
  char buffer[bSize];

  sa.sa_handler = cleaner;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  if(mkfifo("clientToServer", 0666) == -1 && errno != EEXIST){
    perror("mkfifo");
    return EXIT_FAILURE;
  }
  if(mkfifo("serverToClient", 0666) == -1 && errno != EEXIST){
    perror("mkfifo");
    unlink("clientToServer");
    return EXIT_FAILURE;
  }

  printf("Server started. Waiting for yapping...\n");
  fdWrite = open("serverToClient", O_WRONLY);
  if(fdWrite < 0){
    if(!stop_requested) perror("open");
    cleanup_resources();
    return stop_requested ? 0 : EXIT_FAILURE;
  }
  fdRead = open("clientToServer", O_RDONLY);
  if(fdRead < 0){
    if(!stop_requested) perror("open");
    cleanup_resources();
    return stop_requested ? 0 : EXIT_FAILURE;
  }

  while(!stop_requested){
    // clear the buffer
    memset(buffer, 0, bSize);
    int bytesRead = read(fdRead, buffer, bSize);
    if(bytesRead < 0){
      if(stop_requested && errno == EINTR){
        break;
      }
      perror("read");
      break;
    }
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

  cleanup_resources();
  return 0;
}
