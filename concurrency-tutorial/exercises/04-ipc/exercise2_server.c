/*
 * Exercise 2: Named Pipe Chat - Server
 *
 * Objective: Create a chat server using named pipes (FIFOs) that can
 * communicate with a separate client process.
 *
 * Requirements:
 * - Create two named pipes: "client_to_server" and "server_to_client"
 * - Open both pipes (one for reading, one for writing)
 * - Read messages from client and echo them back
 * - Handle SIGINT for cleanup (Ctrl+C)
 * - Unlink (remove) FIFOs on exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define FIFO_CLIENT_TO_SERVER "client_to_server"
#define FIFO_SERVER_TO_CLIENT "server_to_client"
#define BUFFER_SIZE 1024

// TODO: Declare global file descriptors for the FIFOs
// int fd_read, fd_write;

// TODO: Implement signal handler for cleanup
// void cleanup_handler(int signum) {
//     - Print cleanup message
//     - Close both file descriptors
//     - Unlink both FIFOs
//     - Exit
// }

int main() {
    // TODO: Setup signal handler for SIGINT

    // TODO: Create two named pipes using mkfifo()
    // Check for errors (note: EEXIST is okay if pipes already exist)

    printf("Server: Creating FIFOs...\n");

    // TODO: Open the FIFOs
    // - Open FIFO_SERVER_TO_CLIENT for writing (O_WRONLY)
    // - Open FIFO_CLIENT_TO_SERVER for reading (O_RDONLY)
    // Note: Opening may block until the other end is opened

    printf("Server: Ready for communication...\n");

    // TODO: Implement main communication loop
    // while(1) {
    //     - Clear buffer
    //     - Read from client
    //     - If bytes read > 0:
    //         * Print the message
    //         * Echo it back to client
    //     - If bytes read == 0:
    //         * Client disconnected, reopen if needed
    //     - Handle errors
    // }

    // TODO: Cleanup (if loop exits)
    // - Close file descriptors
    // - Unlink FIFOs

    return 0;
}
