/*
 * Exercise 5 Solution: Unix Domain Socket Echo Server
 *
 * This server uses Unix domain sockets to communicate with multiple clients.
 * Each client is handled in a separate thread for concurrent operation.
 *
 * Key concepts:
 * - Creating Unix domain sockets with socket(AF_UNIX, ...)
 * - Binding to a socket file path
 * - Listening for connections
 * - Accepting multiple clients
 * - Thread-based concurrent handling
 * - Proper cleanup of socket files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/ipc_exercise_socket"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5

// Global server socket for cleanup
int server_fd = -1;

/*
 * Signal handler for cleanup
 * Called when user presses Ctrl+C
 */
void cleanup_handler(int signum) {
    printf("\nServer: Shutting down...\n");

    // Close server socket
    if (server_fd != -1) {
        close(server_fd);
    }

    // Remove socket file from filesystem
    unlink(SOCKET_PATH);

    printf("Server: Cleanup complete. Goodbye!\n");
    exit(0);
}

/*
 * Thread function to handle a client connection
 * Each client runs in its own thread
 */
void* handle_client(void* arg) {
    // Get client socket descriptor from argument
    int client_fd = *(int*)arg;
    free(arg);  // Free the allocated memory for socket descriptor

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("Server: New client connected (socket %d)\n", client_fd);

    // Communication loop with this client
    while (1) {
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Read message from client
        bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

        if (bytes_read > 0) {
            // Null-terminate and print the message
            buffer[bytes_read] = '\0';
            printf("Server: Received from client %d: %s\n", client_fd, buffer);

            // Echo the message back to client
            ssize_t bytes_written = write(client_fd, buffer, bytes_read);
            if (bytes_written <= 0) {
                perror("Server: write failed");
                break;
            }
        }
        else if (bytes_read == 0) {
            // Client disconnected
            printf("Server: Client %d disconnected\n", client_fd);
            break;
        }
        else {
            // Read error
            perror("Server: read failed");
            break;
        }
    }

    // Close client socket
    close(client_fd);
    printf("Server: Client %d connection closed\n", client_fd);

    return NULL;
}

int main() {
    struct sockaddr_un server_addr;
    int client_fd;

    // Setup signal handler for graceful shutdown
    signal(SIGINT, cleanup_handler);

    printf("Server: Starting up...\n");

    // Create Unix domain socket
    // AF_UNIX: Unix domain socket (local communication)
    // SOCK_STREAM: Stream socket (connection-oriented, like TCP)
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    printf("Server: Socket created\n");

    // Remove any existing socket file
    // This is important - leftover socket files will cause bind() to fail
    unlink(SOCKET_PATH);

    // Setup socket address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;  // Unix domain socket
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind socket to the file path
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    printf("Server: Socket bound to %s\n", SOCKET_PATH);

    // Listen for incoming connections
    // MAX_CLIENTS: Maximum number of pending connections in queue
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    printf("Server: Listening for connections...\n");
    printf("Server: Press Ctrl+C to shutdown\n\n");

    // Main accept loop
    // Accept connections and create threads to handle them
    while (1) {
        // Accept a client connection
        // This blocks until a client connects
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("accept");
            continue;  // Continue accepting other clients
        }

        // Allocate memory to pass socket descriptor to thread
        // Each thread gets its own copy of the client socket descriptor
        int* client_fd_ptr = malloc(sizeof(int));
        if (client_fd_ptr == NULL) {
            perror("malloc");
            close(client_fd);
            continue;
        }
        *client_fd_ptr = client_fd;

        // Create a new thread to handle this client
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("pthread_create");
            free(client_fd_ptr);
            close(client_fd);
            continue;
        }

        // Detach the thread so it cleans up automatically when done
        pthread_detach(thread);
    }

    // Cleanup (unreachable in this infinite loop, but good practice)
    cleanup_handler(0);

    return 0;
}
