/*
 * Exercise 2 Solution: Named Pipe Chat - Server
 *
 * This server uses named pipes (FIFOs) to communicate with a client.
 * Named pipes allow unrelated processes to communicate.
 *
 * Key concepts:
 * - Creating FIFOs with mkfifo()
 * - Opening FIFOs for reading and writing
 * - Signal handling for cleanup
 * - Unlinking FIFOs on exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define FIFO_CLIENT_TO_SERVER "client_to_server"
#define FIFO_SERVER_TO_CLIENT "server_to_client"
#define BUFFER_SIZE 1024

// Global file descriptors for cleanup
int fd_read = -1;
int fd_write = -1;

/*
 * Signal handler for cleanup
 * Called when user presses Ctrl+C
 */
void cleanup_handler(int signum) {
    printf("\nServer: Cleaning up and shutting down...\n");

    // Close file descriptors if open
    if (fd_read != -1) {
        close(fd_read);
    }
    if (fd_write != -1) {
        close(fd_write);
    }

    // Remove the FIFOs from the filesystem
    unlink(FIFO_CLIENT_TO_SERVER);
    unlink(FIFO_SERVER_TO_CLIENT);

    printf("Server: Cleanup complete. Goodbye!\n");
    exit(0);
}

int main() {
    char buffer[BUFFER_SIZE];

    // Setup signal handler for graceful shutdown
    signal(SIGINT, cleanup_handler);

    printf("Server: Starting up...\n");

    // Create the named pipes (FIFOs)
    // Note: mkfifo() returns -1 if the FIFO already exists (errno = EEXIST)
    if (mkfifo(FIFO_CLIENT_TO_SERVER, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo client_to_server");
            return 1;
        }
        printf("Server: FIFO %s already exists\n", FIFO_CLIENT_TO_SERVER);
    }

    if (mkfifo(FIFO_SERVER_TO_CLIENT, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo server_to_client");
            unlink(FIFO_CLIENT_TO_SERVER);
            return 1;
        }
        printf("Server: FIFO %s already exists\n", FIFO_SERVER_TO_CLIENT);
    }

    printf("Server: FIFOs created successfully\n");
    printf("Server: Waiting for client connection...\n");

    // Open the FIFOs
    // Note: Opening a FIFO for reading blocks until another process opens it for writing
    // We open the write FIFO first with O_WRONLY to prevent blocking
    fd_write = open(FIFO_SERVER_TO_CLIENT, O_WRONLY);
    if (fd_write == -1) {
        perror("open server_to_client");
        cleanup_handler(0);
    }

    fd_read = open(FIFO_CLIENT_TO_SERVER, O_RDONLY);
    if (fd_read == -1) {
        perror("open client_to_server");
        cleanup_handler(0);
    }

    printf("Server: Client connected. Ready for communication...\n");

    // Main communication loop
    while (1) {
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Read message from client
        ssize_t bytes_read = read(fd_read, buffer, BUFFER_SIZE - 1);

        if (bytes_read > 0) {
            // Received a message
            printf("Client: %s\n", buffer);

            // Echo the message back to client
            ssize_t bytes_written = write(fd_write, buffer, bytes_read);
            if (bytes_written <= 0) {
                perror("Server: write failed");
                break;
            }
        }
        else if (bytes_read == 0) {
            // Client closed the write end of the pipe
            printf("Server: Client disconnected\n");

            // Close and reopen to accept a new client
            close(fd_read);
            printf("Server: Waiting for new client...\n");
            fd_read = open(FIFO_CLIENT_TO_SERVER, O_RDONLY);
            if (fd_read < 0) {
                perror("Server: reopen failed");
                break;
            }
            printf("Server: New client connected\n");
        }
        else {
            // Read error
            perror("Server: read failed");
            break;
        }
    }

    // Cleanup
    cleanup_handler(0);

    return 0;
}
