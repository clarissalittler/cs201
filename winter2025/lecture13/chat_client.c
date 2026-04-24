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
}

int main() {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    struct sigaction sa = {0};
    
    // Set up signal handler
    sa.sa_handler = cleanup;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    
    printf("Connecting to chat server...\n");
    
    // Open the FIFOs
    server_fd = open(FIFO_SERVER, O_WRONLY);
    if (server_fd == -1) {
        if (!stop_requested) perror("open");
        cleanup_resources();
        return stop_requested ? 0 : EXIT_FAILURE;
    }
    client_fd = open(FIFO_CLIENT, O_RDONLY);
    if (client_fd == -1) {
        if (!stop_requested) perror("open");
        cleanup_resources();
        return stop_requested ? 0 : EXIT_FAILURE;
    }
    
    printf("Connected to server. Start chatting!\n");
    
    // Read welcome message
    memset(buffer, 0, BUFFER_SIZE);
    bytes_read = read(client_fd, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);
    
    // Chat loop
    while (!stop_requested) {
        // Get client message
        printf("Your message: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            if (stop_requested || feof(stdin)) {
                break;
            }
            clearerr(stdin);
            continue;
        }
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
        if (bytes_read < 0) {
            if (stop_requested && errno == EINTR) {
                break;
            }
            perror("read");
            break;
        }
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
    
    cleanup_resources();
    return 0;
}
