#include <stdio.h>      // Provides input/output functions like printf
#include <stdlib.h>     // Provides general utilities like memory management
#include <string.h>     // Provides string manipulation functions like strlen
#include <unistd.h>     // Provides UNIX system calls like close()
#include <arpa/inet.h>  // Provides networking functions and structures for internet operations
#include <pthread.h>
#include <signal.h>

const int serverPort = 8080;
const int bufferSize = 1024;


int clientSocket;


void cleanup(int signum){
  close(clientSocket);
  exit(0);
}

int main(){
  struct sockaddr_in serverAddr;
  char msg[bufferSize];

  clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  signal(SIGINT,cleanup);

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(serverPort);
  inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

  connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
  while(1){
    printf("You: ");
    fgets(msg, bufferSize, stdin);
    write(clientSocket, msg, strlen(msg));
    read(clientSocket, msg, bufferSize);
    printf("Server: %s", msg);
  }


  close(clientSocket);
  return 0;
}
