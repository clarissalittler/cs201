/*
 * Socket Exercise 5 Solution: Chat Room Server
 * 
 * This solution implements a multi-client chat server using select()
 * Features:
 * - Single-process concurrent handling
 * - User nicknames
 * - Broadcast messages
 * - Private messages
 * - User listing
 * - Join/leave notifications
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
    struct sockaddr_in addr;
} Client;

Client clients[MAX_CLIENTS];
int server_socket;

// Forward declarations
void broadcast_message(char *message, int sender_index);

void init_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
        clients[i].socket = -1;
        memset(clients[i].nickname, 0, NICKNAME_SIZE);
    }
}

int add_client(int socket, struct sockaddr_in addr) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].socket = socket;
            clients[i].active = 1;
            clients[i].addr = addr;
            return i;
        }
    }
    return -1;
}

void remove_client(int index) {
    if (index < 0 || index >= MAX_CLIENTS || !clients[index].active) {
        return;
    }
    
    // Broadcast leave message if client had a nickname
    if (strlen(clients[index].nickname) > 0) {
        char message[BUFFER_SIZE];
        snprintf(message, BUFFER_SIZE, "User %s has left the chat\n", 
                 clients[index].nickname);
        broadcast_message(message, -1);
    }
    
    close(clients[index].socket);
    clients[index].active = 0;
    clients[index].socket = -1;
    memset(clients[index].nickname, 0, NICKNAME_SIZE);
}

void broadcast_message(char *message, int sender_index) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && i != sender_index) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
}

void send_user_list(int client_index) {
    char list[BUFFER_SIZE] = "Connected users:\n";
    int count = 0;
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strlen(clients[i].nickname) > 0) {
            strcat(list, "  - ");
            strcat(list, clients[i].nickname);
            if (i == client_index) {
                strcat(list, " (you)");
            }
            strcat(list, "\n");
            count++;
        }
    }
    
    if (count == 0) {
        strcpy(list, "No users connected\n");
    }
    
    send(clients[client_index].socket, list, strlen(list), 0);
}

void send_private_message(int sender_index, char *recipient, char *message) {
    char formatted[BUFFER_SIZE];
    int found = 0;
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && 
            strcmp(clients[i].nickname, recipient) == 0) {
            snprintf(formatted, BUFFER_SIZE, "[Private from %s]: %s\n", 
                     clients[sender_index].nickname, message);
            send(clients[i].socket, formatted, strlen(formatted), 0);
            
            // Confirm to sender
            snprintf(formatted, BUFFER_SIZE, "[Private to %s]: %s\n", 
                     recipient, message);
            send(clients[sender_index].socket, formatted, strlen(formatted), 0);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        snprintf(formatted, BUFFER_SIZE, "User '%s' not found\n", recipient);
        send(clients[sender_index].socket, formatted, strlen(formatted), 0);
    }
}

void handle_command(char *command, int client_index) {
    char *cmd = strtok(command, " ");
    
    if (strcmp(cmd, "/list") == 0) {
        send_user_list(client_index);
    } else if (strcmp(cmd, "/msg") == 0) {
        char *recipient = strtok(NULL, " ");
        char *message = strtok(NULL, "");
        
        if (recipient && message) {
            send_private_message(client_index, recipient, message);
        } else {
            char *error = "Usage: /msg <nickname> <message>\n";
            send(clients[client_index].socket, error, strlen(error), 0);
        }
    } else if (strcmp(cmd, "/quit") == 0) {
        char *bye = "Goodbye!\n";
        send(clients[client_index].socket, bye, strlen(bye), 0);
        remove_client(client_index);
    } else {
        char *error = "Unknown command. Available: /list, /msg, /quit\n";
        send(clients[client_index].socket, error, strlen(error), 0);
    }
}

void handle_client_message(int client_index) {
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + NICKNAME_SIZE + 10];
    int bytes_read;
    
    bytes_read = recv(clients[client_index].socket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_read <= 0) {
        // Client disconnected
        remove_client(client_index);
        return;
    }
    
    buffer[bytes_read] = '\0';
    // Remove newline
    if (buffer[bytes_read - 1] == '\n') {
        buffer[bytes_read - 1] = '\0';
    }
    
    // Check if client needs to set nickname
    if (strlen(clients[client_index].nickname) == 0) {
        // First message is nickname
        strncpy(clients[client_index].nickname, buffer, NICKNAME_SIZE - 1);
        clients[client_index].nickname[NICKNAME_SIZE - 1] = '\0';
        
        // Send welcome and broadcast join
        snprintf(message, sizeof(message), "Welcome to the chat, %s!\n", 
                 clients[client_index].nickname);
        send(clients[client_index].socket, message, strlen(message), 0);
        
        snprintf(message, sizeof(message), "User %s has joined the chat\n", 
                 clients[client_index].nickname);
        broadcast_message(message, client_index);
        return;
    }
    
    // Handle commands
    if (buffer[0] == '/') {
        handle_command(buffer, client_index);
        return;
    }
    
    // Regular message - broadcast to all
    snprintf(message, sizeof(message), "[%s]: %s\n", 
             clients[client_index].nickname, buffer);
    broadcast_message(message, client_index);
}

int main() {
    fd_set read_fds, master_fds;
    int max_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int opt = 1;
    
    init_clients();
    
    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Bind and listen
    if (bind(server_socket, (struct sockaddr *)&server_addr, 
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // Initialize fd_set
    FD_ZERO(&master_fds);
    FD_SET(server_socket, &master_fds);
    max_fd = server_socket;
    
    printf("Chat server running on port %d\n", PORT);
    printf("Clients should send their nickname as the first message\n");
    
    while (1) {
        // Copy master set to read set
        read_fds = master_fds;
        
        // Wait for activity
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            if (errno == EINTR) continue;
            perror("Select failed");
            break;
        }
        
        // Check for new connections
        if (FD_ISSET(server_socket, &read_fds)) {
            int new_socket = accept(server_socket, 
                                   (struct sockaddr *)&client_addr, 
                                   &client_len);
            if (new_socket >= 0) {
                int index = add_client(new_socket, client_addr);
                if (index >= 0) {
                    FD_SET(new_socket, &master_fds);
                    if (new_socket > max_fd) {
                        max_fd = new_socket;
                    }
                    
                    char *prompt = "Please enter your nickname: ";
                    send(new_socket, prompt, strlen(prompt), 0);
                    
                    printf("New connection from %s:%d\n",
                           inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port));
                } else {
                    char *full = "Server full, please try again later\n";
                    send(new_socket, full, strlen(full), 0);
                    close(new_socket);
                }
            }
        }
        
        // Check each client for data
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && 
                FD_ISSET(clients[i].socket, &read_fds)) {
                handle_client_message(i);
                
                // If client was removed, update fd_set
                if (!clients[i].active) {
                    FD_CLR(clients[i].socket, &master_fds);
                }
            }
        }
    }
    
    // Cleanup
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            close(clients[i].socket);
        }
    }
    close(server_socket);
    
    return 0;
}