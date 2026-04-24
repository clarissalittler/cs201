// chat_server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define FIFO_SERVER "fifo_server"
#define FIFO_CLIENT "fifo_client"
#define BUFFER_SIZE 256

int server_fd = -1, client_fd = -1;
volatile sig_atomic_t stop_requested = 0;

void cleanup(int sig) {
    static const char msg[] = "\nCleaning up and exiting...\n";
    (void)sig;
    stop_requested = 1;
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void cleanup_resources(void) {
    if (server_fd != -1) close(server_fd);
    if (client_fd != -1) close(client_fd);
    unlink(FIFO_SERVER);
    unlink(FIFO_CLIENT);
}

int main() {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    struct sigaction sa = {0};
    
    // Set up signal handler
    sa.sa_handler = cleanup;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    
    // Create named pipes
    if (mkfifo(FIFO_SERVER, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return EXIT_FAILURE;
    }
    if (mkfifo(FIFO_CLIENT, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        unlink(FIFO_SERVER);
        return EXIT_FAILURE;
    }
    
    printf("Chat Server started. Waiting for connections...\n");
    
    server_fd = open(FIFO_SERVER, O_RDONLY);
    if (server_fd == -1) {
        if (!stop_requested) perror("open");
        cleanup_resources();
        return stop_requested ? 0 : EXIT_FAILURE;
    }
    client_fd = open(FIFO_CLIENT, O_WRONLY);
    if (client_fd == -1) {
        if (!stop_requested) perror("open");
        cleanup_resources();
        return stop_requested ? 0 : EXIT_FAILURE;
    }
    
    printf("Client connected. Start chatting!\n");
    
    // Send welcome message
    strcpy(buffer, "Server: Welcome to the chat!");
    write(client_fd, buffer, strlen(buffer) + 1);
    
    // Chat loop
    while (!stop_requested) {
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);
        
        // Read message from client
        bytes_read = read(server_fd, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            if (stop_requested && errno == EINTR) {
                break;
            }
            perror("read");
            break;
        }
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
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            if (stop_requested || feof(stdin)) {
                break;
            }
            clearerr(stdin);
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
        
        // Send message to client
        write(client_fd, buffer, strlen(buffer) + 1);
        
        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }
    
    cleanup_resources();
    return 0;
}
