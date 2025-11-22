/*
 * Exercise 2 Solution: Named Pipe Chat - Client
 *
 * This client connects to the server using named pipes (FIFOs).
 * It uses a separate thread to read responses from the server.
 *
 * Key concepts:
 * - Opening existing FIFOs
 * - Using threads for concurrent reading/writing
 * - Non-blocking communication
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#define FIFO_CLIENT_TO_SERVER "client_to_server"
#define FIFO_SERVER_TO_CLIENT "server_to_client"
#define BUFFER_SIZE 1024

// Global file descriptors
int fd_read;
int fd_write;

/*
 * Thread function to read messages from server
 * Runs concurrently with the main thread that sends messages
 */
void* read_from_server(void* arg) {
    char buffer[BUFFER_SIZE];

    while (1) {
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Read from server
        ssize_t bytes_read = read(fd_read, buffer, BUFFER_SIZE - 1);

        if (bytes_read > 0) {
            // Display the echoed message from server
            printf("Server: %s\n", buffer);
            printf("You: ");
            fflush(stdout);  // Force output to display immediately
        }
        else if (bytes_read == 0) {
            // Server closed connection
            printf("\nClient: Server disconnected\n");
            return NULL;
        }
        else {
            // Read error
            perror("Client: read failed");
            return NULL;
        }
    }

    return NULL;
}

int main() {
    char message[BUFFER_SIZE];
    pthread_t read_thread;

    printf("Client: Connecting to server...\n");

    // Open the FIFOs
    // Note: Open in opposite direction from server
    // Client writes to client_to_server, reads from server_to_client
    fd_read = open(FIFO_SERVER_TO_CLIENT, O_RDONLY);
    if (fd_read == -1) {
        perror("open server_to_client");
        fprintf(stderr, "Client: Make sure server is running first!\n");
        return 1;
    }

    fd_write = open(FIFO_CLIENT_TO_SERVER, O_WRONLY);
    if (fd_write == -1) {
        perror("open client_to_server");
        close(fd_read);
        return 1;
    }

    printf("Client: Connected to server.\n");
    printf("Type your messages below (Ctrl+C to exit):\n\n");

    // Create a thread to read from server
    if (pthread_create(&read_thread, NULL, read_from_server, NULL) != 0) {
        perror("pthread_create");
        close(fd_read);
        close(fd_write);
        return 1;
    }

    // Main loop: read user input and send to server
    printf("You: ");
    fflush(stdout);

    while (fgets(message, BUFFER_SIZE, stdin) != NULL) {
        // Remove trailing newline
        message[strcspn(message, "\n")] = '\0';

        // Skip empty messages
        if (strlen(message) == 0) {
            printf("You: ");
            fflush(stdout);
            continue;
        }

        // Send message to server
        ssize_t bytes_written = write(fd_write, message, strlen(message));
        if (bytes_written <= 0) {
            perror("Client: write failed");
            break;
        }

        printf("You: ");
        fflush(stdout);
    }

    // Cleanup
    printf("\nClient: Closing connection...\n");
    close(fd_write);
    close(fd_read);

    // Wait for read thread to finish (may need to be cancelled)
    pthread_cancel(read_thread);
    pthread_join(read_thread, NULL);

    return 0;
}
