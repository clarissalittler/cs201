/*
 * Exercise 5: Chat Room Server
 * Difficulty: 🔴 Advanced
 * 
 * OBJECTIVE:
 * Create a multi-client chat server where messages from one client
 * are broadcast to all other connected clients. Use select() or poll()
 * for handling multiple clients in a single process.
 * 
 * REQUIREMENTS:
 * 1. Server listens on port 8080
 * 2. Support up to 10 simultaneous clients
 * 3. Clients identify themselves with a nickname
 * 4. Features to implement:
 *    - Broadcast join/leave notifications
 *    - Prefix messages with sender's nickname
 *    - List connected users with "/list" command
 *    - Private messages with "/msg nickname message"
 *    - Quit with "/quit"
 * 5. Use select() to handle multiple clients without blocking
 * 
 * PROTOCOL:
 * Join: "User Alice has joined the chat"
 * Message: "[Alice]: Hello everyone!"
 * Private: "[Private from Alice]: Hi Bob!"
 * Leave: "User Alice has left the chat"
 * 
 * HINTS:
 * - Maintain array of client structures (socket, nickname, etc.)
 * - Use fd_set and select() to monitor multiple sockets
 * - Remember to add/remove clients from fd_set
 * - Parse commands starting with '/'
 * 
 * ADVANCED FEATURES (Optional):
 * - Message history for new joiners
 * - Nickname validation (unique, alphanumeric)
 * - Idle timeout for inactive clients
 * - Rate limiting to prevent spam
 * 
 * DATA STRUCTURES:
 * Consider using a client structure like:
 * typedef struct {
 *     int socket;
 *     char nickname[32];
 *     int active;
 * } Client;
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define NICKNAME_SIZE 32

typedef struct {
    int socket;
    char nickname[NICKNAME_SIZE];
    int active;
} Client;

Client clients[MAX_CLIENTS];
int server_socket;

void init_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
        clients[i].socket = -1;
    }
}

int add_client(int socket) {
    // TODO: Find empty slot and add client
    return -1;
}

void remove_client(int index) {
    // TODO: Remove client and broadcast leave message
}

void broadcast_message(char *message, int sender_index) {
    // TODO: Send message to all active clients except sender
}

void handle_command(char *command, int client_index) {
    // TODO: Parse and handle commands like /list, /msg, /quit
}

void handle_client_message(int client_index) {
    // TODO: Read message from client and broadcast or handle command
}

int main() {
    fd_set read_fds, master_fds;
    int max_fd;
    
    init_clients();
    
    // TODO: Create server socket, bind, listen
    
    // TODO: Initialize fd_set with server socket
    
    printf("Chat server running on port %d\n", PORT);
    
    while (1) {
        // TODO: Copy master set to read set
        
        // TODO: Call select()
        
        // TODO: Check for new connections on server socket
        
        // TODO: Check each client socket for data
        
        // TODO: Handle client messages/disconnections
    }
    
    return 0;
}