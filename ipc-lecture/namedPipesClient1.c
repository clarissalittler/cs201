#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

const int bSize = 1024;

int fdWrite, fdRead;

void *readFromServer(void *arg){
  char buffer[bSize];
  while(1){
    memset(buffer, 0, bSize);
    int bytesRead = read(fdRead,buffer,bSize);
    if(bytesRead > 0){
      printf("Server: %s\n", buffer);
      printf("You: ");
      fflush(stdout);
    }
    else if(bytesRead == 0){
      return NULL;
    }
  }
}

int main(){
  char message[bSize];
  pthread_t thread;

  fdRead = open("serverToClient", O_RDONLY);
  fdWrite = open("clientToServer", O_WRONLY);

  pthread_create(&thread, NULL, readFromServer,NULL);

  printf("Connected to the chat. Type your messages below.\nYou: ");

  while(fgets(message, bSize, stdin) != NULL){
    message[strcspn(message, "\n")] = '\0';
    if (strlen(message) == 0) continue;
    // Write the message to the server
    if (write(fdWrite, message, strlen(message)) < 0) {
      break;
    }
    printf("You: ");
    fflush(stdout); // needed for when you don't have a \n in the printf
  }

  close(fdWrite);
  close(fdRead);
  return 0;
}
