#include <stdio.h>      // Provides input/output functions like printf
#include <stdlib.h>     // Provides general utilities like memory management
#include <string.h>     // Provides string manipulation functions like strlen
#include <unistd.h>     // Provides UNIX system calls like close()
#include <arpa/inet.h>  // Provides networking functions and structures for internet operations
#include <pthread.h>

char buffer[1024];  // Message buffer with greeting

void* echoHandler(void* arg){

  int* newSocket = (int*)arg;

  while(1) {
    memset(buffer, 0, 1024);  // Clear buffer
    int valread = read(*newSocket, buffer, 1024);  // Read message from client
    if (valread > 0) {
      printf("Received: %s\n", buffer);  // Print received message
      write(*newSocket, buffer, strlen(buffer));  // Echo message back to client
    }
    else {
      break;  // Exit loop if no more data is received
    }
  }

  return NULL;  
}

int main(){

  int port = 8080;       // The port number the server will listen on

  int serverFd;         // File descriptor for the server socket
  int newSocket;        // File descriptor for the accepted client connection
  struct sockaddr_in address;  // Structure to hold server address information
  int addrlen = sizeof(address);  // Size of the address structure

  serverFd = socket(AF_INET, SOCK_STREAM,0);
  // Set up address structure
  address.sin_family = AF_INET;  // IPv4
  address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address
  address.sin_port = htons(port);  // Set port number, converting to network byte order

    // Bind the socket to the address and port
  bind(serverFd, (struct sockaddr *)&address, sizeof(address));
  listen(serverFd, 3);

  printf("Server listening on port %d\n", port);
  while(1){
    newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    pthread_t thread;
    pthread_create(&thread,NULL,echoHandler, &newSocket);
    pthread_detach(thread);
  }


  close(newSocket);  // Close client socket
  close(serverFd);  // Close server socket

  return 0;

}
