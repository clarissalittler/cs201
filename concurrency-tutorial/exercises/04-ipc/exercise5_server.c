/*
 * Exercise 5: Unix Domain Socket Echo Server
 *
 * Objective: Create a multi-client echo server using Unix domain sockets
 * with thread-based concurrency.
 *
 * Requirements:
 * - Create a Unix domain socket
 * - Bind to a socket file path
 * - Listen for connections
 * - Accept multiple clients concurrently
 * - Handle each client in a separate thread
 * - Echo messages back to clients
 * - Implement cleanup on shutdown
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

// TODO: Declare global server socket descriptor
// int server_fd;

// TODO: Implement cleanup handler
// void cleanup_handler(int signum) {
//     - Print cleanup message
//     - Close server socket
//     - Unlink socket file
//     - Exit
// }

// TODO: Implement client handler thread function
// void* handle_client(void* arg) {
//     - Get client socket from argument
//     - Declare buffer
//     - Loop:
//         * Clear buffer
//         * Read from client
//         * If bytes read > 0:
//             - Print received message
//             - Echo back to client
//         * If bytes read <= 0:
//             - Client disconnected, break
//     - Close client socket
//     - Free the socket descriptor memory
//     - Return NULL
// }

int main() {
    // TODO: Setup signal handler for SIGINT

    // TODO: Create Unix domain socket
    // server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    // Check for errors

    // TODO: Remove any existing socket file
    // unlink(SOCKET_PATH);

    // TODO: Setup socket address structure
    // struct sockaddr_un server_addr;
    // memset(&server_addr, 0, sizeof(server_addr));
    // server_addr.sun_family = AF_UNIX;
    // strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // TODO: Bind socket to address
    // bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // Check for errors

    // TODO: Listen for connections
    // listen(server_fd, MAX_CLIENTS);
    // Check for errors

    printf("Server: Listening on %s\n", SOCKET_PATH);

    // TODO: Main accept loop
    // while(1) {
    //     - Accept client connection
    //     - Allocate memory for client socket descriptor
    //     - Create thread to handle client
    //     - Detach thread
    // }

    // TODO: Cleanup (if loop exits)
    // - Close server socket
    // - Unlink socket file

    return 0;
}
