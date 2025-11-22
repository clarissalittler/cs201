/*
 * Exercise 2: Named Pipe Chat - Client
 *
 * Objective: Create a chat client that connects to the server using
 * named pipes (FIFOs).
 *
 * Requirements:
 * - Open existing named pipes created by server
 * - Send user input to server
 * - Receive and display server responses in a separate thread
 * - Handle graceful shutdown
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

// TODO: Declare global file descriptors
// int fd_read, fd_write;

// TODO: Implement thread function to read from server
// void* read_from_server(void* arg) {
//     - Declare buffer
//     - Loop continuously:
//         * Clear buffer
//         * Read from server
//         * If bytes read > 0, print message
//         * If bytes read == 0, return (server disconnected)
//     - Return NULL
// }

int main() {
    // TODO: Open the FIFOs (note: opposite directions from server)
    // - Open FIFO_CLIENT_TO_SERVER for writing
    // - Open FIFO_SERVER_TO_CLIENT for reading

    printf("Client: Connected to server.\n");

    // TODO: Create thread for reading from server
    // pthread_t read_thread;
    // pthread_create(&read_thread, NULL, read_from_server, NULL);

    // TODO: Main loop for sending messages
    // - Declare message buffer
    // - Prompt user for input
    // - Read user input with fgets()
    // - Remove newline character
    // - Write message to server
    // - Handle write errors

    // TODO: Cleanup
    // - Close file descriptors

    return 0;
}
