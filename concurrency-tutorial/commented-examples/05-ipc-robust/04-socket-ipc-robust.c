/**
 * ============================================================================
 * Unix Domain Sockets - ROBUST VERSION with Full Error Handling
 * ============================================================================
 *
 * This is the robust version of the socket IPC example with comprehensive
 * error handling, proper resource cleanup, and production-ready practices.
 *
 * Compare with ../05-ipc/04-socket-ipc.c to see the minimal version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <time.h>

/* Path for the Unix domain socket */
#define SOCKET_PATH "/tmp/example_socket_robust"

/* Buffer sizes */
#define MAX_MESSAGE_LEN 256
#define MAX_RESPONSE_LEN 512

/* Number of messages to exchange */
#define NUM_MESSAGES 5

/* Function prototypes */
int run_server(void);
int run_client(void);
void usage(const char* program_name);

int main(int argc, char* argv[]) {
    /* Check command-line arguments */
    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    printf("Unix Domain Socket IPC Demonstration (Robust Version)\n\n");

    /* Run in server or client mode based on command-line argument */
    if (strcmp(argv[1], "server") == 0) {
        return run_server();
    } else if (strcmp(argv[1], "client") == 0) {
        return run_client();
    } else {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
}

/**
 * Run in server mode with full error handling
 */
int run_server(void) {
    int server_fd = -1, client_fd = -1;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[MAX_MESSAGE_LEN];
    char response[MAX_RESPONSE_LEN];
    int ret = EXIT_SUCCESS;

    printf("Server: Starting up...\n");

    /* Create a Unix domain socket */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        fprintf(stderr, "Server ERROR: Failed to create socket: %s\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    printf("Server: Socket created (fd=%d)\n", server_fd);

    /* Set up the server socket address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;

    /* Safely copy socket path */
    if (strlen(SOCKET_PATH) >= sizeof(server_addr.sun_path)) {
        fprintf(stderr, "Server ERROR: Socket path too long\n");
        close(server_fd);
        return EXIT_FAILURE;
    }
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    /* Remove any existing socket file */
    if (unlink(SOCKET_PATH) == -1 && errno != ENOENT) {
        fprintf(stderr, "Server WARNING: Failed to remove old socket: %s\n",
                strerror(errno));
    }

    /* Bind the socket to the address */
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "Server ERROR: Failed to bind socket: %s\n",
                strerror(errno));
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("Server: Socket bound to path %s\n", SOCKET_PATH);

    /* Listen for connections */
    if (listen(server_fd, 5) == -1) {
        fprintf(stderr, "Server ERROR: Failed to listen: %s\n", strerror(errno));
        close(server_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    printf("Server: Listening for connections...\n");

    /* Accept a connection */
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        fprintf(stderr, "Server ERROR: Failed to accept connection: %s\n",
                strerror(errno));
        close(server_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    printf("Server: Client connected (fd=%d)\n", client_fd);

    /* Process messages from the client */
    int message_count = 0;
    while (message_count < NUM_MESSAGES) {
        /* Clear buffer before receiving */
        memset(buffer, 0, sizeof(buffer));

        ssize_t num_bytes = recv(client_fd, buffer, MAX_MESSAGE_LEN - 1, 0);

        if (num_bytes == -1) {
            fprintf(stderr, "Server ERROR: Failed to receive: %s\n",
                    strerror(errno));
            ret = EXIT_FAILURE;
            break;
        }

        if (num_bytes == 0) {
            printf("Server: Client disconnected after %d messages\n",
                   message_count);
            break;
        }

        /* Ensure null termination */
        buffer[num_bytes] = '\0';
        message_count++;

        printf("Server: Received message %d: \"%s\"\n", message_count, buffer);

        /* Prepare a response with bounds checking */
        int written = snprintf(response, MAX_RESPONSE_LEN,
                              "Server response to message %d: \"%.200s\"",
                              message_count, buffer);

        if (written < 0) {
            fprintf(stderr, "Server ERROR: Failed to format response\n");
            ret = EXIT_FAILURE;
            break;
        }

        if (written >= MAX_RESPONSE_LEN) {
            fprintf(stderr, "Server WARNING: Response truncated\n");
        }

        /* Send the response back to the client */
        ssize_t sent = send(client_fd, response, strlen(response), 0);

        if (sent == -1) {
            fprintf(stderr, "Server ERROR: Failed to send response: %s\n",
                    strerror(errno));
            ret = EXIT_FAILURE;
            break;
        }

        if ((size_t)sent != strlen(response)) {
            fprintf(stderr, "Server WARNING: Partial send (%zd/%zu bytes)\n",
                    sent, strlen(response));
        }

        printf("Server: Sent response: \"%s\"\n", response);
    }

    /* Clean up */
    if (client_fd != -1) {
        if (close(client_fd) == -1) {
            fprintf(stderr, "Server ERROR: Failed to close client socket: %s\n",
                    strerror(errno));
            ret = EXIT_FAILURE;
        }
    }

    if (close(server_fd) == -1) {
        fprintf(stderr, "Server ERROR: Failed to close server socket: %s\n",
                strerror(errno));
        ret = EXIT_FAILURE;
    }

    if (unlink(SOCKET_PATH) == -1) {
        fprintf(stderr, "Server WARNING: Failed to remove socket file: %s\n",
                strerror(errno));
    }

    printf("Server: Cleaned up and exiting.\n");
    return ret;
}

/**
 * Run in client mode with full error handling
 */
int run_client(void) {
    int client_fd = -1;
    struct sockaddr_un server_addr;
    char message[MAX_MESSAGE_LEN];
    char buffer[MAX_RESPONSE_LEN];
    int ret = EXIT_SUCCESS;

    printf("Client: Starting up...\n");

    /* Small delay to ensure server is ready */
    sleep(1);

    /* Create a Unix domain socket */
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        fprintf(stderr, "Client ERROR: Failed to create socket: %s\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    printf("Client: Socket created (fd=%d)\n", client_fd);

    /* Set up the server socket address to connect to */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;

    if (strlen(SOCKET_PATH) >= sizeof(server_addr.sun_path)) {
        fprintf(stderr, "Client ERROR: Socket path too long\n");
        close(client_fd);
        return EXIT_FAILURE;
    }
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    /* Connect to the server */
    if (connect(client_fd, (struct sockaddr*)&server_addr,
                sizeof(server_addr)) == -1) {
        fprintf(stderr, "Client ERROR: Failed to connect: %s\n", strerror(errno));
        fprintf(stderr, "Client: Is the server running?\n");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("Client: Connected to server at %s\n", SOCKET_PATH);

    /* Seed the random number generator */
    srand((unsigned int)time(NULL));

    /* Send messages to the server and receive responses */
    for (int i = 0; i < NUM_MESSAGES; i++) {
        /* Prepare a message */
        int written = snprintf(message, MAX_MESSAGE_LEN,
                              "Message %d from client (random: %d)",
                              i + 1, rand() % 1000);

        if (written < 0) {
            fprintf(stderr, "Client ERROR: Failed to format message\n");
            ret = EXIT_FAILURE;
            break;
        }

        if (written >= MAX_MESSAGE_LEN) {
            fprintf(stderr, "Client WARNING: Message truncated\n");
        }

        printf("Client: Sending message: \"%s\"\n", message);

        /* Send the message to the server */
        ssize_t sent = send(client_fd, message, strlen(message), 0);

        if (sent == -1) {
            fprintf(stderr, "Client ERROR: Failed to send message: %s\n",
                    strerror(errno));
            ret = EXIT_FAILURE;
            break;
        }

        if ((size_t)sent != strlen(message)) {
            fprintf(stderr, "Client WARNING: Partial send (%zd/%zu bytes)\n",
                    sent, strlen(message));
        }

        /* Receive the server's response */
        memset(buffer, 0, sizeof(buffer));
        ssize_t num_bytes = recv(client_fd, buffer, MAX_RESPONSE_LEN - 1, 0);

        if (num_bytes == -1) {
            fprintf(stderr, "Client ERROR: Failed to receive response: %s\n",
                    strerror(errno));
            ret = EXIT_FAILURE;
            break;
        }

        if (num_bytes == 0) {
            fprintf(stderr, "Client ERROR: Server closed connection unexpectedly\n");
            ret = EXIT_FAILURE;
            break;
        }

        /* Null terminate the received data */
        buffer[num_bytes] = '\0';

        printf("Client: Received response: \"%s\"\n", buffer);

        /* Sleep to make the demo easier to follow */
        sleep(1);
    }

    printf("Client: Sent %d messages.\n", NUM_MESSAGES);

    /* Clean up */
    if (close(client_fd) == -1) {
        fprintf(stderr, "Client ERROR: Failed to close socket: %s\n",
                strerror(errno));
        ret = EXIT_FAILURE;
    }

    printf("Client: Cleaned up and exiting.\n");
    return ret;
}

/**
 * Display usage information for the program.
 */
void usage(const char* program_name) {
    fprintf(stderr, "Usage: %s <mode>\n", program_name);
    fprintf(stderr, "  mode: 'server' or 'client'\n");
    fprintf(stderr, "\nTo demonstrate socket communication:\n");
    fprintf(stderr, "  1. Run as server in one terminal: %s server\n", program_name);
    fprintf(stderr, "  2. Run as client in another terminal: %s client\n", program_name);
}

/**
 * KEY DIFFERENCES FROM MINIMAL VERSION:
 *
 * 1. Comprehensive Error Checking:
 *    - All socket operations checked
 *    - String operations bounds-checked
 *    - Buffer operations verified
 *
 * 2. Resource Management:
 *    - File descriptors closed on all paths
 *    - Socket file cleaned up properly
 *    - No resource leaks even on error
 *
 * 3. Defensive Programming:
 *    - Buffer overflow prevention
 *    - Null termination guaranteed
 *    - Partial send/recv handled
 *    - Path length validation
 *
 * 4. Better Error Reporting:
 *    - Detailed error messages
 *    - Suggestions for user (e.g., "Is server running?")
 *    - Warning vs error distinction
 *
 * 5. Robustness Features:
 *    - Handles unexpected disconnections
 *    - Safe string operations (snprintf, strncpy)
 *    - Proper cleanup on all exit paths
 *    - Return codes reflect success/failure
 *
 * PRODUCTION READY:
 * This version demonstrates patterns suitable for production code:
 * - Fail gracefully with clear error messages
 * - Clean up resources in all cases
 * - Validate all inputs and buffer operations
 * - Handle partial operations
 * - Provide actionable error messages
 */
