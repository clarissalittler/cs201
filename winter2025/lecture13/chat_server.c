// chat_server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

#define FIFO_SERVER "fifo_server"
#define FIFO_CLIENT "fifo_client"
#define BUFFER_SIZE 256

int server_fd = -1, client_fd = -1;

void cleanup(int sig) {
    (void)sig;  // Suppress unused parameter warning
    printf("\nCleaning up and exiting...\n");
    if (server_fd != -1) close(server_fd);
    if (client_fd != -1) close(client_fd);
    unlink(FIFO_SERVER);
    unlink(FIFO_CLIENT);
    exit(0);
}

int main() {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    
    // Set up signal handler
    signal(SIGINT, cleanup);
    
    // Create named pipes
    mkfifo(FIFO_SERVER, 0666);
    mkfifo(FIFO_CLIENT, 0666);
    
    printf("Chat Server started. Waiting for connections...\n");
    
    server_fd = open(FIFO_SERVER, O_RDONLY);
    client_fd = open(FIFO_CLIENT, O_WRONLY);
    
    printf("Client connected. Start chatting!\n");
    
    // Send welcome message
    strcpy(buffer, "Server: Welcome to the chat!");
    write(client_fd, buffer, strlen(buffer) + 1);
    
    // Chat loop
    while (1) {
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);
        
        // Read message from client
        bytes_read = read(server_fd, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        
        printf("Client: %s\n", buffer);
        
        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        
        // Get server response
        printf("Your response: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
        
        // Send message to client
        write(client_fd, buffer, strlen(buffer) + 1);
        
        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }
    
    cleanup(0);
    return 0;
}
