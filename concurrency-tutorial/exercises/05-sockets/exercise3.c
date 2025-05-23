/*
 * Exercise 3: Multi-Client Server
 * Difficulty: 🟡 Intermediate
 * 
 * OBJECTIVE:
 * Extend the echo server to handle multiple clients sequentially.
 * After serving one client, the server should continue accepting
 * new connections until interrupted.
 * 
 * REQUIREMENTS:
 * 1. Create a server that runs in an infinite loop
 * 2. Accept client connections one at a time
 * 3. For each client:
 *    - Read messages in a loop until client disconnects
 *    - Echo each message back with a prefix "[ECHO] "
 *    - Print client activity to server console
 * 4. Handle client disconnection gracefully
 * 5. Continue accepting new clients
 * 
 * EXPECTED OUTPUT:
 * Server: Listening on port 8080...
 * Server: Client 1 connected
 * Server: Client 1 sent: Hello
 * Server: Client 1 sent: How are you?
 * Server: Client 1 disconnected
 * Server: Client 2 connected
 * ...
 * 
 * HINTS:
 * - Use nested loops: outer for connections, inner for messages
 * - recv() returns 0 when client closes connection
 * - recv() returns -1 on error
 * - Consider adding client numbering for clarity
 * 
 * COMMON PITFALLS:
 * - Not handling partial sends/receives
 * - Memory leaks from not closing sockets
 * - Not checking for interrupted system calls
 * 
 * BONUS CHALLENGE:
 * Add signal handling (SIGINT) for graceful server shutdown
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define LISTEN_BACKLOG 5

int server_socket = -1;  // Global for signal handler

void handle_sigint(int sig) {
    printf("\nServer: Shutting down gracefully...\n");
    if (server_socket >= 0) {
        close(server_socket);
    }
    exit(0);
}

int main() {
    int client_count = 0;
    
    // TODO: Set up signal handler for graceful shutdown
    
    // TODO: Create TCP socket
    
    // TODO: Set SO_REUSEADDR option
    
    // TODO: Setup server address and bind
    
    // TODO: Listen for connections
    
    printf("Server: Listening on port %d...\n", PORT);
    printf("Server: Press Ctrl+C to shutdown\n");
    
    // TODO: Main server loop
    while (1) {
        // TODO: Accept client connection
        
        client_count++;
        printf("Server: Client %d connected\n", client_count);
        
        // TODO: Message handling loop for this client
        while (1) {
            // TODO: Receive message from client
            
            // TODO: Check if client disconnected
            
            // TODO: Add "[ECHO] " prefix and send back
            
            // TODO: Print what was received
        }
        
        // TODO: Close client socket
        printf("Server: Client %d disconnected\n", client_count);
    }
    
    return 0;
}