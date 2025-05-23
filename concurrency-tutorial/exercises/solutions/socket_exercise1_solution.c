/*
 * Socket Exercise 1 Solution: Simple Echo Server
 * 
 * This solution demonstrates the basic steps to create a TCP server:
 * 1. Create socket
 * 2. Set socket options
 * 3. Bind to address and port
 * 4. Listen for connections
 * 5. Accept a client
 * 6. Read and echo data
 * 7. Clean up
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int bytes_read;
    int opt = 1;
    
    // Create TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set SO_REUSEADDR to allow quick restart
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // Setup server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, 
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_socket, 1) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Server: Listening on port %d...\n", PORT);
    
    // Accept a client connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, 
                          &client_len);
    if (client_socket < 0) {
        perror("Accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Server: Client connected\n");
    
    // Read message from client
    memset(buffer, 0, BUFFER_SIZE);
    bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read < 0) {
        perror("Receive failed");
    } else if (bytes_read == 0) {
        printf("Server: Client disconnected\n");
    } else {
        // Remove newline if present
        if (buffer[bytes_read - 1] == '\n') {
            buffer[bytes_read - 1] = '\0';
        }
        
        printf("Server: Received: %s\n", buffer);
        
        // Echo message back to client
        if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
        } else {
            printf("Server: Echoed back to client\n");
        }
    }
    
    // Close sockets
    close(client_socket);
    close(server_socket);
    
    return 0;
}