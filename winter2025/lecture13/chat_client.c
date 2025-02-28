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

int server_fd, client_fd;

void cleanup(int sig) {
    printf("\nCleaning up and exiting...\n");
    if (server_fd != -1) close(server_fd);
    if (client_fd != -1) close(client_fd);
    exit(0);
}

int main() {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    
    // Set up signal handler
    signal(SIGINT, cleanup);
    
    printf("Connecting to chat server...\n");
    
    // Open the FIFOs
    server_fd = open(FIFO_SERVER, O_WRONLY);
    client_fd = open(FIFO_CLIENT, O_RDONLY);
    
    printf("Connected to server. Start chatting!\n");
    
    // Read welcome message
    memset(buffer, 0, BUFFER_SIZE);
    bytes_read = read(client_fd, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);
    
    // Chat loop
    while (1) {
        // Get client message
        printf("Your message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
        
        // Send message to server
        write(server_fd, buffer, strlen(buffer) + 1);
        
        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);
        
        // Read response from server
        bytes_read = read(client_fd, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        
        printf("Server: %s\n", buffer);
        
        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }
    
    cleanup(0);
    return 0;
}
