/*
 * Socket Exercise 4 Solution: Concurrent Server with Processes
 * 
 * This solution demonstrates:
 * - Fork-based concurrent server
 * - Proper file descriptor management
 * - Zombie process prevention
 * - Simple protocol with client identification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_NAME_LEN 50
#define LISTEN_BACKLOG 10

// Signal handler to reap zombie children
void sigchld_handler(int sig) {
    (void)sig;
    // Reap all available zombie children
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char client_name[MAX_NAME_LEN];
    char response[BUFFER_SIZE];
    int bytes_read;
    
    // Read client's name (first message)
    bytes_read = recv(client_socket, client_name, MAX_NAME_LEN - 1, 0);
    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }
    
    // Remove newline if present
    client_name[bytes_read] = '\0';
    if (client_name[bytes_read - 1] == '\n') {
        client_name[bytes_read - 1] = '\0';
    }
    
    // Send welcome message
    snprintf(response, BUFFER_SIZE, "Welcome, %s!\n", client_name);
    send(client_socket, response, strlen(response), 0);
    
    printf("Server: Client '%s' joined (PID: %d)\n", client_name, getpid());
    
    // Echo loop with name prefix
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_read <= 0) {
            // Client disconnected or error
            break;
        }
        
        // Remove newline
        if (buffer[bytes_read - 1] == '\n') {
            buffer[bytes_read - 1] = '\0';
        }
        
        // Check for quit command
        if (strcmp(buffer, "quit") == 0) {
            snprintf(response, BUFFER_SIZE, "Goodbye, %s!\n", client_name);
            send(client_socket, response, strlen(response), 0);
            break;
        }
        
        // Create response with name prefix
        snprintf(response, BUFFER_SIZE, "[%s]: %s\n", client_name, buffer);
        
        // Send response
        if (send(client_socket, response, strlen(response), 0) < 0) {
            perror("Send failed");
            break;
        }
    }
    
    printf("Server: Client '%s' disconnected (PID: %d)\n", client_name, getpid());
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pid_t pid;
    int opt = 1;
    
    // Set up signal handler for SIGCHLD
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    
    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set SO_REUSEADDR
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
    
    // Bind and listen
    if (bind(server_socket, (struct sockaddr *)&server_addr, 
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_socket, LISTEN_BACKLOG) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Server: Concurrent server listening on port %d\n", PORT);
    printf("Server: Protocol: Send name first, then messages. Type 'quit' to exit.\n");
    
    // Main accept loop
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, 
                              &client_len);
        if (client_socket < 0) {
            if (errno == EINTR) {
                continue;  // Interrupted by signal, retry
            }
            perror("Accept failed");
            continue;
        }
        
        printf("Server: New connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
        
        // Fork to handle client
        pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(client_socket);
            continue;
        }
        
        if (pid == 0) {
            // Child process
            close(server_socket);  // Child doesn't need server socket
            handle_client(client_socket);
            exit(0);
        } else {
            // Parent process
            close(client_socket);  // Parent doesn't need client socket
        }
    }
    
    close(server_socket);
    return 0;
}