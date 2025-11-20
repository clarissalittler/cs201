/**
 * ============================================================================
 * Socket IPC - Flexible Network-Capable Communication
 * ============================================================================
 * 
 * This example demonstrates using Unix domain sockets for inter-process
 * communication. Sockets provide a flexible, bidirectional communication
 * mechanism that works with a consistent interface for both local and
 * network communication.
 * 
 * Key concepts demonstrated:
 * - Creating Unix domain sockets
 * - Client-server communication pattern
 * - Bidirectional message exchange
 * - Socket cleanup
 * 
 * This file contains both client and server functionality that can be selected
 * at runtime to demonstrate communication between separate processes.
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
#define SOCKET_PATH "/tmp/example_socket"

/* Buffer sizes */
#define MAX_MESSAGE_LEN 256
#define MAX_RESPONSE_LEN 256

/* Number of messages to exchange */
#define NUM_MESSAGES 5

/* Function prototypes */
void run_server();
void run_client();
void usage(const char* program_name);

int main(int argc, char* argv[]) {
    /* Check command-line arguments */
    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    printf("Unix Domain Socket IPC Demonstration\n\n");
    
    /* Run as server or client based on command-line argument */
    if (strcmp(argv[1], "server") == 0) {
        run_server();
    } else if (strcmp(argv[1], "client") == 0) {
        run_client();
    } else {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/**
 * Run in server mode: Create a Unix domain socket, listen for connections,
 * and respond to client messages.
 */
void run_server() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[MAX_MESSAGE_LEN];
    char response[MAX_RESPONSE_LEN];
    int i, num_bytes;
    
    printf("Server: Starting up...\n");
    
    /* Create a Unix domain socket */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    printf("Server: Socket created.\n");
    
    /* 
     * Set up the server socket address
     * AF_UNIX indicates this is a Unix domain socket
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    /* Remove any existing socket file */
    unlink(SOCKET_PATH);
    
    /* Bind the socket to the address */
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Server: Socket bound to path %s\n", SOCKET_PATH);
    
    /* Listen for connections */
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }
    
    printf("Server: Listening for connections...\n");
    
    /* Accept a connection */
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }
    
    printf("Server: Client connected.\n");
    
    /* Process messages from the client */
    i = 0;
    while ((num_bytes = recv(client_fd, buffer, MAX_MESSAGE_LEN - 1, 0)) > 0) {
        i++;
        
        /* Null terminate the received data */
        buffer[num_bytes] = '\0';
        
        printf("Server: Received message %d: \"%s\"\n", i, buffer);

        /* Prepare a response (limit buffer to avoid truncation) */
        snprintf(response, MAX_RESPONSE_LEN, "Server response to \"%.200s\"", buffer);

        /* Send the response back to the client */
        if (send(client_fd, response, strlen(response), 0) == -1) {
            perror("send");
            break;
        }
        
        printf("Server: Sent response: \"%s\"\n", response);
        
        /* Exit after receiving the expected number of messages */
        if (i >= NUM_MESSAGES) {
            printf("Server: Received all expected messages.\n");
            break;
        }
    }
    
    if (num_bytes == -1) {
        perror("recv");
    } else if (num_bytes == 0) {
        printf("Server: Client disconnected.\n");
    }
    
    /* Clean up */
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    
    printf("Server: Cleaned up and exiting.\n");
}

/**
 * Run in client mode: Connect to the server socket and send messages.
 */
void run_client() {
    int client_fd;
    struct sockaddr_un server_addr;
    char message[MAX_MESSAGE_LEN];
    char buffer[MAX_RESPONSE_LEN];
    int i, num_bytes;
    
    printf("Client: Starting up...\n");
    
    /* Create a Unix domain socket */
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    printf("Client: Socket created.\n");
    
    /* Set up the server socket address to connect to */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    /* Connect to the server */
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Client: Connected to server at %s\n", SOCKET_PATH);
    
    /* Seed the random number generator */
    srand(time(NULL));
    
    /* Send messages to the server and receive responses */
    for (i = 0; i < NUM_MESSAGES; i++) {
        /* Prepare a message */
        snprintf(message, MAX_MESSAGE_LEN, "Message %d from client (random: %d)", 
                 i + 1, rand() % 1000);
        
        printf("Client: Sending message: \"%s\"\n", message);
        
        /* Send the message to the server */
        if (send(client_fd, message, strlen(message), 0) == -1) {
            perror("send");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
        
        /* Receive the server's response */
        num_bytes = recv(client_fd, buffer, MAX_RESPONSE_LEN - 1, 0);
        if (num_bytes == -1) {
            perror("recv");
            close(client_fd);
            exit(EXIT_FAILURE);
        } else if (num_bytes == 0) {
            fprintf(stderr, "Client: Server closed connection unexpectedly.\n");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
        
        /* Null terminate the received data */
        buffer[num_bytes] = '\0';
        
        printf("Client: Received response: \"%s\"\n", buffer);
        
        /* Sleep to make the demo easier to follow */
        sleep(1);
    }
    
    printf("Client: Sent all %d messages.\n", NUM_MESSAGES);
    
    /* Clean up */
    close(client_fd);
    
    printf("Client: Cleaned up and exiting.\n");
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
 * EXPLANATION:
 * 
 * Unix Domain Sockets - Flexible IPC:
 * 
 * 1. What Are Unix Domain Sockets?
 *    - IPC mechanism for local communication between processes
 *    - Similar API to network sockets (consistent programming model)
 *    - Identified by pathnames in the filesystem
 *    - Generally faster than Internet sockets for local communication
 *    - Support both stream (connection-oriented) and datagram (connectionless) modes
 * 
 * 2. Unix Domain Sockets vs. Internet Sockets:
 *    - Both use the same socket API
 *    - Unix domain sockets are faster for local communication
 *    - Unix domain sockets use filesystem paths instead of IP/port
 *    - Unix domain sockets can pass file descriptors between processes
 *    - Unix domain sockets are limited to local machine communication
 * 
 * 3. Socket Types:
 *    - SOCK_STREAM: Reliable, connection-oriented, sequenced (like TCP)
 *    - SOCK_DGRAM: Connectionless, message-oriented (like UDP)
 *    - SOCK_SEQPACKET: Reliable, connection-oriented, message-oriented
 *    - This example uses SOCK_STREAM for reliable, ordered communication
 * 
 * 4. Client-Server Pattern:
 *    - Server: Creates socket, binds to an address, listens, and accepts connections
 *    - Client: Creates socket, connects to server's address
 *    - Both client and server can send and receive data after connection
 * 
 * 5. Key Socket Operations:
 *    - socket(): Create a new socket
 *    - bind(): Associate a socket with an address
 *    - listen(): Mark a socket as passive (for accepting connections)
 *    - accept(): Accept an incoming connection
 *    - connect(): Connect to a remote socket
 *    - send()/recv(): Send/receive data
 *    - close(): Close a socket
 * 
 * 6. Unix Domain Socket Addresses:
 *    - Use struct sockaddr_un with AF_UNIX family
 *    - Contains a pathname in the filesystem (sun_path)
 *    - Socket appears as a special file (with type 's')
 *    - Must be unlinked (deleted) when no longer needed
 * 
 * 7. Advanced Features:
 *    - File descriptor passing (SCM_RIGHTS)
 *    - Credential passing (SCM_CREDENTIALS)
 *    - Abstract namespace sockets (Linux-specific)
 *    - Non-blocking I/O
 *    - Support for select()/poll()/epoll() for monitoring
 * 
 * 8. Common Use Cases:
 *    - Local client-server applications
 *    - Database connections (e.g., PostgreSQL, MySQL)
 *    - X Window System
 *    - D-Bus message system
 *    - System services and daemons
 * 
 * 9. Advantages:
 *    - Bidirectional communication
 *    - Familiar socket API
 *    - Good performance for local communication
 *    - Support for passing file descriptors
 *    - Ability to use select/poll/epoll for monitoring multiple connections
 * 
 * 10. Running This Example:
 *    - Start the server in one terminal
 *    - Start the client in another terminal
 *    - Observe the bidirectional message exchange
 *    - Server cleans up the socket file when done
 */