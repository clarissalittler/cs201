// chat_server_pm_commented.c
// Chat server with usernames and private messaging.
// This builds on chat_server.c, adding:
//   - User authentication (username selection)
//   - Command parsing
//   - Private messaging (targeted communication)
//   - User directory functionality
//
// Compile: gcc -o chat_server_pm_commented chat_server_pm_commented.c -pthread
// Usage: ./chat_server_pm_commented port
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

// ============================================================================
// CLIENT STRUCTURE WITH USERNAME
// ============================================================================
// Extended from chat_server.c to include a username field.
typedef struct {
    int fd;
    int active;
    char username[MAX_USERNAME];   // NEW: Each client has a username
    char ip[INET_ADDRSTRLEN];
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Forward declarations
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
        clients[i].username[0] = '\0';  // Empty string = no username yet
    }

    // Socket setup (same as before)
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

// ============================================================================
// CLIENT HANDLER WITH USERNAME AUTHENTICATION
// ============================================================================
void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    ssize_t bytes;

    // ========================================================================
    // USERNAME REGISTRATION FLOW
    // ========================================================================
    // Before the client can chat, they must choose a username.
    // This demonstrates a simple authentication/registration protocol.

    // Prompt for username
    char *prompt = "Enter your username: ";
    send(client_fd, prompt, strlen(prompt), 0);

    // Receive username
    // NOTE: Using MAX_USERNAME-1 to ensure room for null terminator
    bytes = recv(client_fd, buffer, MAX_USERNAME - 1, 0);
    if (bytes <= 0) {
        // Client disconnected before sending username
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }
    buffer[bytes] = '\0';

    // ====================================================================
    // STRING PROCESSING: TRIM WHITESPACE
    // ====================================================================
    // User input often has unwanted whitespace (newlines, spaces).
    // trim() removes leading/trailing whitespace.
    trim(buffer);

    // ====================================================================
    // VALIDATION: REJECT INVALID USERNAMES
    // ====================================================================
    // Always validate user input! Never trust it.

    // Check for empty username after trimming
    if (strlen(buffer) == 0) {
        char *msg = "Invalid username. Disconnecting.\n";
        send(client_fd, msg, strlen(msg), 0);
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }

    // Check if username is already taken
    // This prevents impersonation and confusion.
    if (username_exists(buffer)) {
        char *msg = "Username already taken. Disconnecting.\n";
        send(client_fd, msg, strlen(msg), 0);
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }

    // Username is valid! Save it.
    set_username(client_fd, buffer);
    char *username = get_username(client_fd);

    // ====================================================================
    // SEND WELCOME AND HELP
    // ====================================================================
    char welcome[512];
    snprintf(welcome, sizeof(welcome),
             "\nWelcome, %s!\n"
             "Commands:\n"
             "  @username message  - Private message\n"
             "  /who               - List users\n"
             "  /quit              - Disconnect\n\n",
             username);
    send(client_fd, welcome, strlen(welcome), 0);

    // Announce to other clients that this user joined
    char announce[256];
    snprintf(announce, sizeof(announce), "*** %s joined the chat ***\n", username);
    printf("%s", announce);
    broadcast(announce, client_fd);

    // ========================================================================
    // MAIN MESSAGE LOOP WITH COMMAND PARSING
    // ========================================================================
    while ((bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        trim(buffer);

        if (strlen(buffer) == 0) continue;

        // ====================================================================
        // COMMAND: /quit
        // ====================================================================
        if (strcmp(buffer, "/quit") == 0) {
            break;  // Exit the loop, which will disconnect the client
        }

        // ====================================================================
        // COMMAND: /who
        // ====================================================================
        if (strcmp(buffer, "/who") == 0) {
            send_user_list(client_fd);
            continue;  // Don't broadcast this command
        }

        // ====================================================================
        // PRIVATE MESSAGE: @username message
        // ====================================================================
        // This demonstrates parsing a more complex command format.
        if (buffer[0] == '@') {
            // Find the first space, which separates username from message
            char *space = strchr(buffer, ' ');

            if (space == NULL) {
                // No space found - malformed command
                char *err = "Usage: @username message\n";
                send(client_fd, err, strlen(err), 0);
                continue;
            }

            // Split the string at the space
            *space = '\0';  // Replace space with null terminator

            // Now buffer points to "@username\0message"
            char *target_user = buffer + 1;  // Skip the '@' character
            char *message = space + 1;        // Start after the null terminator

            // Validate both parts exist
            if (strlen(target_user) == 0 || strlen(message) == 0) {
                char *err = "Usage: @username message\n";
                send(client_fd, err, strlen(err), 0);
                continue;
            }

            // Send the private message
            send_private(target_user, username, message, client_fd);
            continue;  // Don't broadcast private messages
        }

        // ====================================================================
        // REGULAR MESSAGE: BROADCAST TO ALL
        // ====================================================================
        char message[BUFFER_SIZE + MAX_USERNAME + 8];
        snprintf(message, sizeof(message), "[%s] %s\n", username, buffer);
        printf("%s", message);
        broadcast(message, client_fd);
    }

    // ========================================================================
    // CLIENT LEAVING
    // ========================================================================
    char *user = get_username(client_fd);
    char leave[256];
    snprintf(leave, sizeof(leave), "*** %s left the chat ***\n", user);
    printf("%s", leave);
    broadcast(leave, client_fd);

    remove_client(client_fd);
    close(client_fd);
    return NULL;
}

// ============================================================================
// BROADCAST (unchanged from chat_server.c)
// ============================================================================
void broadcast(char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].fd != sender_fd) {
            send(clients[i].fd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// ============================================================================
// PRIVATE MESSAGE: Send to specific user
// ============================================================================
void send_private(char *to_user, char *from_user, char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);

    int found = 0;

    // Search for the target username
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, to_user) == 0) {
            // Found the recipient!

            // Format message for recipient
            char pm[BUFFER_SIZE + MAX_USERNAME + 16];
            snprintf(pm, sizeof(pm), "[PM from %s] %s\n", from_user, message);
            send(clients[i].fd, pm, strlen(pm), 0);

            // Send confirmation to sender
            // This is good UX - let the sender know their message was delivered.
            char confirm[BUFFER_SIZE + MAX_USERNAME + 16];
            snprintf(confirm, sizeof(confirm), "[PM to %s] %s\n", to_user, message);
            send(sender_fd, confirm, strlen(confirm), 0);

            found = 1;
            break;  // Found the user, no need to continue searching
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    // If user wasn't found, send error message
    if (!found) {
        char err[128];
        snprintf(err, sizeof(err), "User '%s' not found.\n", to_user);
        send(sender_fd, err, strlen(err), 0);
    }
}

// ============================================================================
// SEND USER LIST
// ============================================================================
void send_user_list(int client_fd) {
    // Build a list of all connected users
    char list[BUFFER_SIZE] = "Connected users:\n";

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strlen(clients[i].username) > 0) {
            strcat(list, "  ");
            strcat(list, clients[i].username);
            strcat(list, "\n");

            // SECURITY NOTE: This code uses strcat() which is vulnerable to
            // buffer overflow if there are many users with long names.
            // Production code should use strncat() or track remaining buffer size.
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    send(client_fd, list, strlen(list), 0);
}

// ============================================================================
// ADD CLIENT (modified to not set username initially)
// ============================================================================
int add_client(int fd, char *ip) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].fd = fd;
            clients[i].active = 1;
            clients[i].username[0] = '\0';  // No username yet
            strncpy(clients[i].ip, ip, INET_ADDRSTRLEN);
            pthread_mutex_unlock(&clients_mutex);
            return 0;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
    return -1;
}

// ============================================================================
// REMOVE CLIENT (unchanged)
// ============================================================================
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

// ============================================================================
// SET USERNAME
// ============================================================================
void set_username(int fd, char *username) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == fd) {
            // strncpy() copies at most MAX_USERNAME-1 characters
            // This prevents buffer overflow if username is too long
            strncpy(clients[i].username, username, MAX_USERNAME - 1);
            clients[i].username[MAX_USERNAME - 1] = '\0';  // Ensure null termination
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

// ============================================================================
// GET USERNAME
// ============================================================================
char *get_username(int fd) {
    // IMPORTANT: This function has a subtle issue!
    // It returns a pointer to a static variable. This means:
    //   1. The value changes on each call (not thread-safe for caller)
    //   2. Only one caller can use the result at a time
    //
    // This works in this code because we immediately use the result,
    // but it's a pattern to be aware of. Better alternatives:
    //   - Pass in a buffer to fill
    //   - Return a dynamically allocated string (caller must free)

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

// ============================================================================
// CHECK IF USERNAME EXISTS
// ============================================================================
int username_exists(char *username) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, username) == 0) {
            pthread_mutex_unlock(&clients_mutex);
            return 1;  // Username exists
        }
    }

    pthread_mutex_unlock(&clients_mutex);
    return 0;  // Username doesn't exist
}

// ============================================================================
// TRIM: Remove leading and trailing whitespace
// ============================================================================
void trim(char *str) {
    // This is a common string processing utility function.

    // Trim trailing whitespace
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' ||
                       str[len - 1] == ' ' || str[len - 1] == '\t')) {
        str[--len] = '\0';  // Replace with null terminator
    }

    // Trim leading whitespace
    char *start = str;
    while (*start && (*start == ' ' || *start == '\t')) {
        start++;
    }

    // If there was leading whitespace, shift the string left
    if (start != str) {
        // memmove() is like memcpy() but handles overlapping memory
        // We need this because source and destination overlap
        memmove(str, start, strlen(start) + 1);  // +1 to include null terminator
    }
}

// ============================================================================
// KEY PROTOCOL DESIGN CONCEPTS:
// ============================================================================
//
// 1. AUTHENTICATION:
//    Simple username-based authentication. No passwords (not secure!).
//    In production, you'd want:
//    - Password hashing (bcrypt, argon2)
//    - TLS/SSL for encryption
//    - Token-based auth (JWT, sessions)
//
// 2. COMMAND PARSING:
//    We parse commands by looking at the first character:
//    - '@' = private message
//    - '/' = command
//    - else = broadcast message
//
//    This is a simple protocol. More robust protocols might use:
//    - JSON or protobuf for structured messages
//    - Length prefixes to handle binary data
//    - Message types/headers
//
// 3. USER EXPERIENCE:
//    Good chat servers provide feedback:
//    - Confirm private messages were sent
//    - Notify when commands succeed/fail
//    - Show who's online
//
// 4. INPUT VALIDATION:
//    Always validate and sanitize user input:
//    - Check for empty strings
//    - Trim whitespace
//    - Reject invalid formats
//    - Check buffer lengths
//
// 5. STRING SAFETY:
//    Common pitfalls in C string handling:
//    - Buffer overflows (use strncpy, snprintf, not strcpy/sprintf)
//    - Missing null terminators (always ensure strings are terminated)
//    - Off-by-one errors (remember the null terminator!)
//
// 6. PROTOCOL STATE MACHINE:
//    This client goes through states:
//    1. CONNECTED (waiting for username)
//    2. AUTHENTICATED (can chat)
//    3. DISCONNECTED
//
//    More complex protocols might have many states.
//
// 7. EXTENSIBILITY:
//    This design makes it easy to add new commands:
//    - Add a new if() branch in the message loop
//    - Implement the command handler function
//    - Update the help text
//
// 8. ERROR HANDLING:
//    Notice how we handle errors:
//    - User not found -> send error message, continue
//    - Invalid command -> send usage hint, continue
//    - Network error -> disconnect
//
//    The key is: don't crash! Always handle errors gracefully.
//
// ============================================================================
