#include <stdio.h>      // Provides input/output functions like printf
#include <stdlib.h>     // Provides general utilities like memory management
#include <string.h>     // Provides string manipulation functions like strlen
#include <unistd.h>     // Provides UNIX system calls like close()
#include <arpa/inet.h>  // Provides networking functions and structures for internet operations

int main(){

  int port = 8080;       // The port number the server will listen on

  int server_fd;         // File descriptor for the server socket
  int new_socket;        // File descriptor for the accepted client connection
  struct sockaddr_in address;  // Structure to hold server address information
  int addrlen = sizeof(address);  // Size of the address structure
  char buffer[1024] = "Hey there y'all\n";  // Message buffer with greeting
  
  server_fd = socket(AF_INET, SOCK_STREAM,0);
  // Arguments:
  // - AF_INET: Use IPv4 internet protocol
  // - SOCK_STREAM: Use TCP protocol (reliable, ordered delivery)
  // - 0: Use default protocol

  // Set up address structure
  address.sin_family = AF_INET;  // IPv4
  address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address
  address.sin_port = htons(port);  // Set port number, converting to network byte order

    // Bind the socket to the address and port
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  // Arguments:
  // - server_fd: The socket to bind
  // - (struct sockaddr *)&address: Cast address structure to generic socket address
  // - sizeof(address): Size of address structure
  listen(server_fd, 3);
  // Arguments:
  // - server_fd: Socket to listen on
  // - 3: Backlog - maximum length of queue for pending connections

  printf("Server listening on port %d\n", port);

  // Accept a connection
  new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
  // Arguments:
  // - server_fd: Socket to accept connections on
  // - (struct sockaddr *)&address: Will store connected client's address info
  // - (socklen_t*)&addrlen: Size of address structure
  // Returns: New socket file descriptor for this specific client connection

  write(new_socket, buffer, strlen(buffer));  // Echo message back to client
  // Arguments:
  // - new_socket: Socket to send data on
  // - buffer: Data to send
  // - strlen(buffer): Length of data to send
  
  close(new_socket);  // Close client socket
  close(server_fd);  // Close server socket
  
  return 0;
}
