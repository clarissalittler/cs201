/*
 * Socket Exercise 3 Solution: Multi-Client Server
 * 
 * This solution demonstrates:
 * - Sequential handling of multiple clients
 * - Graceful shutdown with signal handling
 * - Proper error handling and client management
 * - Message echoing with prefix
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define LISTEN_BACKLOG 5

int server_socket = -1;  // Global for signal handler
volatile sig_atomic_t keep_running = 1;  // Flag for main loop

void handle_sigint(int sig) {
    (void)sig;  // Suppress unused parameter warning
    keep_running = 0;
    printf("\nServer: Shutting down gracefully...\n");
    if (server_socket >= 0) {
        close(server_socket);
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE + 10];  // Extra space for "[ECHO] "
    int bytes_read;
    int client_count = 0;
    int opt = 1;
    
    // Set up signal handler
    signal(SIGINT, handle_sigint);
    
    // Create TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set SO_REUSEADDR option
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, 
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_socket, LISTEN_BACKLOG) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Server: Listening on port %d...\n", PORT);
    printf("Server: Press Ctrl+C to shutdown\n");
    
    // Main server loop
    while (keep_running) {
        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, 
                              &client_len);
        if (client_socket < 0) {
            if (errno == EINTR) {
                // Interrupted by signal, check keep_running
                continue;
            }
            perror("Accept failed");
            continue;
        }
        
        client_count++;
        printf("Server: Client %d connected from %s:%d\n", 
               client_count,
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
        
        // Message handling loop for this client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            
            if (bytes_read < 0) {
                perror("Receive failed");
                break;
            } else if (bytes_read == 0) {
                // Client disconnected
                break;
            }
            
            // Remove trailing newline if present
            if (buffer[bytes_read - 1] == '\n') {
                buffer[bytes_read - 1] = '\0';
                bytes_read--;
            }
            
            printf("Server: Client %d sent: %s\n", client_count, buffer);
            
            // Create response with "[ECHO] " prefix
            snprintf(response, sizeof(response), "[ECHO] %s", buffer);
            
            // Send response back
            if (send(client_socket, response, strlen(response), 0) < 0) {
                perror("Send failed");
                break;
            }
        }
        
        // Close client socket
        close(client_socket);
        printf("Server: Client %d disconnected\n", client_count);
    }
    
    // Cleanup
    if (server_socket >= 0) {
        close(server_socket);
    }
    
    printf("Server: Shutdown complete\n");
    return 0;
}