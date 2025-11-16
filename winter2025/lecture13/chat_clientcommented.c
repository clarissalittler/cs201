// PEDAGOGICAL PURPOSE:
// This program demonstrates a SIMPLE CHAT CLIENT using named pipes (FIFOs).
// Key learning objectives:
// 1. Understanding client-side FIFO communication
// 2. Learning how client connects to existing FIFOs (doesn't create them)
// 3. Understanding the importance of correct FIFO opening order
// 4. Seeing interactive user input with fgets()
// 5. Learning signal handling for cleanup on client side

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

#define FIFO_SERVER "fifo_server"  // Client writes here (Server reads)
#define FIFO_CLIENT "fifo_client"  // Client reads here (Server writes)
#define BUFFER_SIZE 256

int server_fd, client_fd;

// CLEANUP HANDLER:
// Client doesn't unlink FIFOs (server created them, server removes them)
void cleanup(int sig) {
    printf("\nCleaning up and exiting...\n");
    if (server_fd != -1) close(server_fd);
    if (client_fd != -1) close(client_fd);
    exit(0);
}

int main() {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Set up signal handler
    signal(SIGINT, cleanup);

    printf("Connecting to chat server...\n");

    // OPEN FIFOs (CLIENT SIDE):
    // Server must already be running and have created the FIFOs
    //
    // IMPORTANT: Open in opposite order from server
    //   - Server opens FIFO_SERVER for reading first
    //   - Client opens FIFO_SERVER for writing first (matches server)
    //   - This prevents deadlock
    //
    // server_fd: Write TO server
    // client_fd: Read FROM server
    server_fd = open(FIFO_SERVER, O_WRONLY);
    client_fd = open(FIFO_CLIENT, O_RDONLY);

    printf("Connected to server. Start chatting!\n");

    // READ WELCOME MESSAGE FROM SERVER:
    // Server sends a welcome message first
    memset(buffer, 0, BUFFER_SIZE);
    bytes_read = read(client_fd, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);

    // CHAT LOOP:
    // Alternates: client sends, server responds
    while (1) {
        // GET USER INPUT:
        // Client types message
        printf("Your message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        // SEND MESSAGE TO SERVER:
        write(server_fd, buffer, strlen(buffer) + 1);

        // CHECK IF CLIENT WANTS TO EXIT:
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // CLEAR BUFFER FOR RESPONSE:
        memset(buffer, 0, BUFFER_SIZE);

        // READ RESPONSE FROM SERVER:
        bytes_read = read(client_fd, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        // DISPLAY SERVER'S RESPONSE:
        printf("Server: %s\n", buffer);

        // CHECK IF SERVER SENT EXIT:
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    cleanup(0);
    return 0;
}

// EXPECTED OUTPUT (CLIENT SIDE):
// Connecting to chat server...
// Connected to server. Start chatting!
// Server: Welcome to the chat!
// Your message: Hello server!
// Server: Hi client!
// Your message: exit
// Cleaning up and exiting...

// KEY DIFFERENCE FROM SERVER:
// - Client doesn't create FIFOs (server does)
// - Client doesn't unlink FIFOs (server does)
// - Client opens in specific order to match server
// - Client receives welcome message before sending

// TRY IT:
// 1. Run server first: ./chat_server
// 2. Run client: ./chat_client
// 3. Chat!
// 4. EXPERIMENT: Try running client without server (fails)
// 5. CHALLENGE: Add automatic reconnection if server restarts
