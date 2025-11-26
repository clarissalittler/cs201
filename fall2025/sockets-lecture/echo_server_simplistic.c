// echo_server.c
// A simple TCP echo server.
// Compile: gcc -o echo_server echo_server.c
// Usage: ./echo_server port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

  int port = 8080;

  // Step 1: Create socket
  int server_fd = socket(AF_INET,// this is on the internet!
			 SOCK_STREAM, // TCP
			 0); // no options

  // Allow address reuse (helpful during development)
  int optval = 1;
  // why are we passing in the pointer to an int and its size?
  // becaUSE this takes a void* as its 4th argument
  setsockopt(server_fd,
	     SOL_SOCKET,
	     SO_REUSEADDR,
	     &optval,
	     sizeof(optval));

    // Step 2: Bind to address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    bind(server_fd,
	 (struct sockaddr *)&server_addr,
	 sizeof(server_addr));
      
    // Step 3: Listen for connections
    listen(server_fd, 10);

    printf("Echo server %d listening on port %d...\n", getpid(), port);
    // Step 4: Accept and handle connections
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        // Step 5: Echo loop
        char buffer[1024];
        ssize_t bytes_received;

        while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            // Echo back what we received
	  ssize_t bytes_sent = send(client_fd, buffer, bytes_received, 0);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
