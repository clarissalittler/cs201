/*
 * Exercise 5: Unix Domain Socket Echo Client
 *
 * Objective: Create a client that connects to the Unix domain socket
 * server and sends/receives messages.
 *
 * Requirements:
 * - Connect to server's Unix domain socket
 * - Send user input to server
 * - Receive and display echoed messages
 * - Handle graceful disconnection
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

// TODO: Declare global client socket
// int client_fd;

// TODO: Implement cleanup handler
// void cleanup_handler(int signum) {
//     - Print cleanup message
//     - Close socket
//     - Exit
// }

int main() {
    // TODO: Setup signal handler for SIGINT

    // TODO: Create Unix domain socket
    // client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    // Check for errors

    // TODO: Setup server address structure
    // struct sockaddr_un server_addr;
    // memset(&server_addr, 0, sizeof(server_addr));
    // server_addr.sun_family = AF_UNIX;
    // strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // TODO: Connect to server
    // connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // Check for errors

    printf("Client: Connected to server.\n");
    printf("Type your messages (Ctrl+C to exit):\n\n");

    // TODO: Main communication loop
    // char message[BUFFER_SIZE];
    // while(1) {
    //     - Prompt user
    //     - Read user input with fgets()
    //     - Remove newline
    //     - Send message to server
    //     - If send fails, break
    //     - Clear buffer
    //     - Read echo from server
    //     - If read fails or returns 0, break
    //     - Print echoed message
    // }

    // TODO: Cleanup
    // - Close socket

    return 0;
}
