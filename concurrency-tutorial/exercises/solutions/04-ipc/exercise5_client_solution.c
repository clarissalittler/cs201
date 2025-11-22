/*
 * Exercise 5 Solution: Unix Domain Socket Echo Client
 *
 * This client connects to the Unix domain socket server and
 * sends/receives messages.
 *
 * Key concepts:
 * - Creating Unix domain sockets
 * - Connecting to a server socket
 * - Sending and receiving data
 * - Graceful disconnection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/ipc_exercise_socket"
#define BUFFER_SIZE 1024

// Global client socket for cleanup
int client_fd = -1;

/*
 * Signal handler for cleanup
 * Called when user presses Ctrl+C
 */
void cleanup_handler(int signum) {
    printf("\nClient: Disconnecting...\n");

    // Close client socket
    if (client_fd != -1) {
        close(client_fd);
    }

    printf("Client: Goodbye!\n");
    exit(0);
}

int main() {
    struct sockaddr_un server_addr;
    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Setup signal handler for graceful shutdown
    signal(SIGINT, cleanup_handler);

    printf("Client: Starting up...\n");

    // Create Unix domain socket
    // AF_UNIX: Unix domain socket (local communication)
    // SOCK_STREAM: Stream socket (connection-oriented)
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        return 1;
    }

    printf("Client: Socket created\n");

    // Setup server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connect to server
    printf("Client: Connecting to server at %s...\n", SOCKET_PATH);
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        fprintf(stderr, "Client: Make sure server is running!\n");
        close(client_fd);
        return 1;
    }

    printf("Client: Connected to server successfully.\n");
    printf("========================================\n");
    printf("Echo Client - Type your messages below\n");
    printf("Press Ctrl+C to exit\n");
    printf("========================================\n\n");

    // Main communication loop
    while (1) {
        // Prompt user for input
        printf("You: ");
        fflush(stdout);

        // Read user input
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Remove trailing newline
        message[strcspn(message, "\n")] = '\0';

        // Skip empty messages
        if (strlen(message) == 0) {
            continue;
        }

        // Send message to server
        ssize_t bytes_sent = write(client_fd, message, strlen(message));
        if (bytes_sent <= 0) {
            perror("Client: write failed");
            fprintf(stderr, "Client: Server may have disconnected\n");
            break;
        }

        // Clear response buffer
        memset(response, 0, BUFFER_SIZE);

        // Read echo from server
        ssize_t bytes_read = read(client_fd, response, BUFFER_SIZE - 1);
        if (bytes_read > 0) {
            // Null-terminate and display the response
            response[bytes_read] = '\0';
            printf("Echo: %s\n\n", response);
        }
        else if (bytes_read == 0) {
            // Server closed connection
            printf("\nClient: Server disconnected\n");
            break;
        }
        else {
            // Read error
            perror("Client: read failed");
            break;
        }
    }

    // Cleanup
    printf("\nClient: Closing connection...\n");
    close(client_fd);

    return 0;
}
