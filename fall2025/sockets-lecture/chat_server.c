// chat_server.c
// Group chat server - messages are broadcast to all connected clients.
// Compile: gcc -o chat_server chat_server.c -pthread
// Usage: ./chat_server port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

// Client tracking
typedef struct {
    int fd;
    int active;
    char ip[INET_ADDRSTRLEN];
    int port;
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg);
void broadcast(char *message, int sender_fd);
int add_client(int fd, char *ip, int port);
void remove_client(int fd);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    // Initialize client array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
        clients[i].active = 0;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Chat server listening on port %d...\n", port);
    printf("Connect with: ncat localhost %d\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        int client_port = ntohs(client_addr.sin_port);

        // Add to client list
        if (add_client(client_fd, client_ip, client_port) == -1) {
            char *msg = "Server full. Try again later.\n";
            send(client_fd, msg, strlen(msg), 0);
            close(client_fd);
            continue;
        }

        printf("New connection: %s:%d\n", client_ip, client_port);

        // Welcome message
        char welcome[256];
        snprintf(welcome, sizeof(welcome),
                 "Welcome to the chat! You are %s:%d\n"
                 "Type messages and press Enter to send.\n",
                 client_ip, client_port);
        send(client_fd, welcome, strlen(welcome), 0);

        // Announce to others
        char announce[256];
        snprintf(announce, sizeof(announce), "*** %s:%d joined the chat ***\n",
                 client_ip, client_port);
        broadcast(announce, client_fd);

        // Create handler thread
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("pthread_create");
            remove_client(client_fd);
            close(client_fd);
            free(client_fd_ptr);
            continue;
        }
        pthread_detach(thread);
    }

    return 0;
}

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    // Find this client's info for message prefixing
    char prefix[64] = "Unknown";
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == client_fd && clients[i].active) {
            snprintf(prefix, sizeof(prefix), "%s:%d",
                     clients[i].ip, clients[i].port);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';

        // Remove trailing newline if present
        if (bytes_received > 0 && buffer[bytes_received - 1] == '\n') {
            buffer[bytes_received - 1] = '\0';
        }

        // Skip empty messages
        if (strlen(buffer) == 0) continue;

        // Format and broadcast
        char message[BUFFER_SIZE + 64];
        snprintf(message, sizeof(message), "[%s] %s\n", prefix, buffer);
        printf("%s", message);  // Log to server console
        broadcast(message, client_fd);
    }

    // Client disconnected
    printf("Client %s disconnected.\n", prefix);

    char leave_msg[256];
    snprintf(leave_msg, sizeof(leave_msg), "*** %s left the chat ***\n", prefix);
    broadcast(leave_msg, client_fd);

    remove_client(client_fd);
    close(client_fd);
    return NULL;
}

void broadcast(char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].fd != sender_fd) {
            send(clients[i].fd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

int add_client(int fd, char *ip, int port) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].fd = fd;
            clients[i].active = 1;
            strncpy(clients[i].ip, ip, INET_ADDRSTRLEN);
            clients[i].port = port;
            pthread_mutex_unlock(&clients_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return -1;  // No room
}

void remove_client(int fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == fd) {
            clients[i].active = 0;
            clients[i].fd = -1;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}
