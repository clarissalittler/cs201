// chat_server_pm.c
// Chat server with usernames and private messaging.
// Compile: gcc -o chat_server_pm chat_server_pm.c -pthread
// Usage: ./chat_server_pm port
//
// Commands:
//   @username message  - Send private message to username
//   /who               - List connected users
//   /quit              - Disconnect

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define MAX_USERNAME 32

typedef struct {
    int fd;
    int active;
    char username[MAX_USERNAME];
    char ip[INET_ADDRSTRLEN];
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg);
void broadcast(char *message, int sender_fd);
void send_private(char *to_user, char *from_user, char *message, int sender_fd);
void send_user_list(int client_fd);
int add_client(int fd, char *ip);
void remove_client(int fd);
void set_username(int fd, char *username);
char *get_username(int fd);
int username_exists(char *username);
void trim(char *str);

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
        clients[i].username[0] = '\0';
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

    printf("Chat server (with PM) listening on port %d...\n", port);

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

        if (add_client(client_fd, client_ip) == -1) {
            char *msg = "Server full. Try again later.\n";
            send(client_fd, msg, strlen(msg), 0);
            close(client_fd);
            continue;
        }

        printf("New connection from %s\n", client_ip);

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

    char buffer[BUFFER_SIZE];
    ssize_t bytes;

    // Prompt for username
    char *prompt = "Enter your username: ";
    send(client_fd, prompt, strlen(prompt), 0);

    bytes = recv(client_fd, buffer, MAX_USERNAME - 1, 0);
    if (bytes <= 0) {
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }
    buffer[bytes] = '\0';
    trim(buffer);

    // Validate username
    if (strlen(buffer) == 0) {
        char *msg = "Invalid username. Disconnecting.\n";
        send(client_fd, msg, strlen(msg), 0);
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }

    if (username_exists(buffer)) {
        char *msg = "Username already taken. Disconnecting.\n";
        send(client_fd, msg, strlen(msg), 0);
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }

    set_username(client_fd, buffer);
    char *username = get_username(client_fd);

    // Welcome message
    char welcome[512];
    snprintf(welcome, sizeof(welcome),
             "\nWelcome, %s!\n"
             "Commands:\n"
             "  @username message  - Private message\n"
             "  /who               - List users\n"
             "  /quit              - Disconnect\n\n",
             username);
    send(client_fd, welcome, strlen(welcome), 0);

    // Announce join
    char announce[256];
    snprintf(announce, sizeof(announce), "*** %s joined the chat ***\n", username);
    printf("%s", announce);
    broadcast(announce, client_fd);

    // Main message loop
    while ((bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        trim(buffer);

        if (strlen(buffer) == 0) continue;

        // Handle commands
        if (strcmp(buffer, "/quit") == 0) {
            break;
        }

        if (strcmp(buffer, "/who") == 0) {
            send_user_list(client_fd);
            continue;
        }

        // Private message: @username message
        if (buffer[0] == '@') {
            char *space = strchr(buffer, ' ');
            if (space == NULL) {
                char *err = "Usage: @username message\n";
                send(client_fd, err, strlen(err), 0);
                continue;
            }

            *space = '\0';
            char *target_user = buffer + 1;  // Skip @
            char *message = space + 1;

            if (strlen(target_user) == 0 || strlen(message) == 0) {
                char *err = "Usage: @username message\n";
                send(client_fd, err, strlen(err), 0);
                continue;
            }

            send_private(target_user, username, message, client_fd);
            continue;
        }

        // Regular broadcast message
        char message[BUFFER_SIZE + MAX_USERNAME + 8];
        snprintf(message, sizeof(message), "[%s] %s\n", username, buffer);
        printf("%s", message);
        broadcast(message, client_fd);
    }

    // Client leaving
    char *user = get_username(client_fd);
    char leave[256];
    snprintf(leave, sizeof(leave), "*** %s left the chat ***\n", user);
    printf("%s", leave);
    broadcast(leave, client_fd);

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

void send_private(char *to_user, char *from_user, char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);

    int found = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, to_user) == 0) {
            char pm[BUFFER_SIZE + MAX_USERNAME + 16];
            snprintf(pm, sizeof(pm), "[PM from %s] %s\n", from_user, message);
            send(clients[i].fd, pm, strlen(pm), 0);

            // Confirm to sender
            char confirm[BUFFER_SIZE + MAX_USERNAME + 16];
            snprintf(confirm, sizeof(confirm), "[PM to %s] %s\n", to_user, message);
            send(sender_fd, confirm, strlen(confirm), 0);

            found = 1;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    if (!found) {
        char err[128];
        snprintf(err, sizeof(err), "User '%s' not found.\n", to_user);
        send(sender_fd, err, strlen(err), 0);
    }
}

void send_user_list(int client_fd) {
    char list[BUFFER_SIZE] = "Connected users:\n";

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strlen(clients[i].username) > 0) {
            strcat(list, "  ");
            strcat(list, clients[i].username);
            strcat(list, "\n");
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    send(client_fd, list, strlen(list), 0);
}

int add_client(int fd, char *ip) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].fd = fd;
            clients[i].active = 1;
            clients[i].username[0] = '\0';
            strncpy(clients[i].ip, ip, INET_ADDRSTRLEN);
            pthread_mutex_unlock(&clients_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return -1;
}

void remove_client(int fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == fd) {
            clients[i].active = 0;
            clients[i].fd = -1;
            clients[i].username[0] = '\0';
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void set_username(int fd, char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == fd) {
            strncpy(clients[i].username, username, MAX_USERNAME - 1);
            clients[i].username[MAX_USERNAME - 1] = '\0';
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

char *get_username(int fd) {
    static char username[MAX_USERNAME];
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == fd) {
            strncpy(username, clients[i].username, MAX_USERNAME);
            pthread_mutex_unlock(&clients_mutex);
            return username;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    strcpy(username, "Unknown");
    return username;
}

int username_exists(char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, username) == 0) {
            pthread_mutex_unlock(&clients_mutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return 0;
}

void trim(char *str) {
    // Trim trailing whitespace
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' ||
                       str[len - 1] == ' ' || str[len - 1] == '\t')) {
        str[--len] = '\0';
    }

    // Trim leading whitespace
    char *start = str;
    while (*start && (*start == ' ' || *start == '\t')) {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}
