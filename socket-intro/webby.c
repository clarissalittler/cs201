#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Set up address structure
    address.sin_family = AF_INET;  // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address
    address.sin_port = htons(PORT);  // Set port number, converting to network byte order

    // Bind the socket to the address and port
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen for incoming connections
    listen(server_fd, 3);

    printf("Web server listening on port %d\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen);

        // Read HTTP request
        read(new_socket, buffer, BUFFER_SIZE);  // Read incoming request from client
        printf("Received request:\n%s\n", buffer);  // Print the HTTP request

        // Prepare HTTP response
        char *response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n"
                         "Content-Length: 46\r\n"
                         "\r\n"
                         "<html><body><h1>Hello, World!</h1></body></html>";

        // Send HTTP response
        write(new_socket, response, strlen(response));  // Send response to client

        close(new_socket);  // Close client connection
    }

    close(server_fd);  // Close server socket
    return 0;
}
