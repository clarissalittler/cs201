// chat_server_commented.c
// Group chat server - messages are broadcast to all connected clients.
// This demonstrates SHARED STATE between threads and the need for SYNCHRONIZATION.
//
// Compile: gcc -o chat_server_commented chat_server_commented.c -pthread
// Usage: ./chat_server_commented port

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

// ============================================================================
// SHARED DATA STRUCTURES
// ============================================================================
// Unlike echo_server_threaded where each thread worked independently,
// this server needs threads to SHARE information about connected clients.
// This creates the need for SYNCHRONIZATION.

// Structure to track each connected client
typedef struct {
    int fd;                      // Socket file descriptor (-1 if slot is unused)
    int active;                  // Boolean: is this slot currently in use?
    char ip[INET_ADDRSTRLEN];   // Client's IP address (for logging)
    int port;                    // Client's port number (for logging)
} Client;

// Global array of all clients
// This is shared by ALL threads - the main thread, and all client handler threads!
Client clients[MAX_CLIENTS];

// ============================================================================
// MUTEX FOR THREAD SYNCHRONIZATION
// ============================================================================
// A MUTEX (mutual exclusion lock) prevents multiple threads from accessing
// shared data simultaneously. Think of it like a bathroom key - only one
// person can hold it at a time.
//
// WHY do we need this?
// Consider two threads both calling add_client() simultaneously:
//   Thread 1: Reads clients[5].active (it's 0)
//   Thread 2: Reads clients[5].active (it's 0)
//   Thread 1: Writes to clients[5]
//   Thread 2: Writes to clients[5]
// Result: RACE CONDITION! Both threads used the same slot, one client is lost!
//
// The mutex ensures: "Only one thread can access clients[] at a time."

// PTHREAD_MUTEX_INITIALIZER is a static initializer for mutexes.
// It creates a mutex that's ready to use without calling pthread_mutex_init().
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Forward declarations
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

    // ========================================================================
    // INITIALIZE CLIENT ARRAY
    // ========================================================================
    // Mark all slots as inactive. This is safe to do before any threads exist.
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
        clients[i].active = 0;
    }

    // ========================================================================
    // SOCKET SETUP (same as previous servers)
    // ========================================================================
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

    // ========================================================================
    // ACCEPT CONNECTIONS AND CREATE HANDLER THREADS
    // ========================================================================
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

        // ====================================================================
        // ADD CLIENT TO THE GLOBAL ARRAY
        // ====================================================================
        // add_client() will use the mutex to safely add this client.
        if (add_client(client_fd, client_ip, client_port) == -1) {
            // Server is full!
            char *msg = "Server full. Try again later.\n";
            send(client_fd, msg, strlen(msg), 0);
            close(client_fd);
            continue;
        }

        printf("New connection: %s:%d\n", client_ip, client_port);

        // ====================================================================
        // SEND WELCOME MESSAGE
        // ====================================================================
        char welcome[256];
        snprintf(welcome, sizeof(welcome),
                 "Welcome to the chat! You are %s:%d\n"
                 "Type messages and press Enter to send.\n",
                 client_ip, client_port);
        send(client_fd, welcome, strlen(welcome), 0);

        // ====================================================================
        // ANNOUNCE TO OTHER CLIENTS
        // ====================================================================
        // broadcast() will send this message to all OTHER connected clients.
        char announce[256];
        snprintf(announce, sizeof(announce), "*** %s:%d joined the chat ***\n",
                 client_ip, client_port);
        broadcast(announce, client_fd);  // client_fd = sender, don't send to self

        // ====================================================================
        // CREATE HANDLER THREAD
        // ====================================================================
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("pthread_create");
            remove_client(client_fd);  // Clean up on error
            close(client_fd);
            free(client_fd_ptr);
            continue;
        }
        pthread_detach(thread);
    }

    return 0;
}

// ============================================================================
// CLIENT HANDLER THREAD
// ============================================================================
void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    // ========================================================================
    // FIND THIS CLIENT'S INFO FOR MESSAGE PREFIXING
    // ========================================================================
    // We need to look up this client's IP:port to prefix their messages.
    // This requires accessing the shared clients[] array, so we need the mutex!

    char prefix[64] = "Unknown";

    // LOCK the mutex before accessing clients[]
    pthread_mutex_lock(&clients_mutex);

    // CRITICAL SECTION: Only one thread can be in here at a time
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == client_fd && clients[i].active) {
            snprintf(prefix, sizeof(prefix), "%s:%d",
                     clients[i].ip, clients[i].port);
            break;
        }
    }

    // UNLOCK the mutex when done
    pthread_mutex_unlock(&clients_mutex);

    // IMPORTANT PATTERN:
    //   1. Lock mutex
    //   2. Access/modify shared data
    //   3. Unlock mutex
    //   4. Do everything else OUTSIDE the critical section

    // ========================================================================
    // MESSAGE RECEIVING LOOP
    // ========================================================================
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';

        // Remove trailing newline if present (from user pressing Enter)
        if (bytes_received > 0 && buffer[bytes_received - 1] == '\n') {
            buffer[bytes_received - 1] = '\0';
        }

        // Skip empty messages
        if (strlen(buffer) == 0) continue;

        // ====================================================================
        // FORMAT AND BROADCAST THE MESSAGE
        // ====================================================================
        char message[BUFFER_SIZE + 64];
        snprintf(message, sizeof(message), "[%s] %s\n", prefix, buffer);

        // Log to server console
        printf("%s", message);

        // Send to all other clients
        broadcast(message, client_fd);
    }

    // ========================================================================
    // CLIENT DISCONNECTED - CLEANUP
    // ========================================================================
    printf("Client %s disconnected.\n", prefix);

    // Announce departure to other clients
    char leave_msg[256];
    snprintf(leave_msg, sizeof(leave_msg), "*** %s left the chat ***\n", prefix);
    broadcast(leave_msg, client_fd);

    // Remove from client list (uses mutex internally)
    remove_client(client_fd);
    close(client_fd);

    return NULL;
}

// ============================================================================
// BROADCAST: Send message to all clients except sender
// ============================================================================
void broadcast(char *message, int sender_fd) {
    // This function is called by multiple threads simultaneously!
    // We need to protect access to the clients[] array.

    pthread_mutex_lock(&clients_mutex);

    // CRITICAL SECTION: Iterate through all clients and send
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].fd != sender_fd) {
            // send() is being called while holding the mutex.
            // This is generally OK, but if send() blocks for a long time,
            // it would prevent other threads from accessing clients[].
            //
            // More advanced designs might copy the FDs out, release the lock,
            // then send. But for moderate load, this is fine.
            send(clients[i].fd, message, strlen(message), 0);

            // NOTE: We're ignoring send() errors here! In production, you'd
            // check for errors and possibly remove dead clients.
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

// ============================================================================
// ADD CLIENT: Find empty slot and add client info
// ============================================================================
int add_client(int fd, char *ip, int port) {
    pthread_mutex_lock(&clients_mutex);

    // CRITICAL SECTION: Find first inactive slot
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            // Found an empty slot!
            clients[i].fd = fd;
            clients[i].active = 1;
            strncpy(clients[i].ip, ip, INET_ADDRSTRLEN);
            clients[i].port = port;

            pthread_mutex_unlock(&clients_mutex);
            return 0;  // Success
        }
    }

    pthread_mutex_unlock(&clients_mutex);
    return -1;  // No room (server full)
}

// ============================================================================
// REMOVE CLIENT: Mark slot as inactive
// ============================================================================
void remove_client(int fd) {
    pthread_mutex_lock(&clients_mutex);

    // CRITICAL SECTION: Find and deactivate this client
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == fd) {
            clients[i].active = 0;
            clients[i].fd = -1;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

// ============================================================================
// KEY SYNCHRONIZATION CONCEPTS:
// ============================================================================
//
// 1. RACE CONDITIONS:
//    When multiple threads access shared data without synchronization, the
//    result depends on timing. This is called a "race condition" because
//    the threads are "racing" - whoever gets there first wins.
//
//    Example races in this code without mutexes:
//    - Two clients join simultaneously -> both might get the same slot
//    - One thread reads clients[] while another writes -> reads corrupt data
//    - One thread removes a client while another broadcasts -> sends to closed FD
//
// 2. CRITICAL SECTIONS:
//    Code between pthread_mutex_lock() and pthread_mutex_unlock() is a
//    "critical section" - only one thread can execute it at a time.
//
//    Keep critical sections SMALL! While one thread holds the lock, all other
//    threads trying to acquire it are BLOCKED (waiting). This limits concurrency.
//
// 3. MUTEX LOCK/UNLOCK PATTERN:
//    Always follow this pattern:
//      pthread_mutex_lock(&mutex);
//      // Access shared data
//      pthread_mutex_unlock(&mutex);
//
//    NEVER:
//    - Return while holding a lock (unlock first!)
//    - Call blocking functions while holding a lock (if avoidable)
//    - Forget to unlock (causes DEADLOCK - other threads wait forever)
//
// 4. DEADLOCK:
//    Happens when threads wait for each other circularly:
//    - Thread A holds Lock 1, waits for Lock 2
//    - Thread B holds Lock 2, waits for Lock 1
//    Both wait forever!
//
//    This code avoids deadlock by using only one mutex. With multiple mutexes,
//    always acquire them in the same order.
//
// 5. GRANULARITY:
//    We have ONE mutex protecting ALL clients. This is "coarse-grained locking."
//    - Advantage: Simple, hard to get wrong
//    - Disadvantage: Less concurrency (threads block each other more)
//
//    "Fine-grained locking" would use one lock per client, allowing concurrent
//    access to different clients. But it's more complex and error-prone.
//
// 6. WHY NOT volatile?:
//    You might wonder why we use mutexes instead of volatile (like in chat_client).
//    - volatile ensures VISIBILITY (changes are seen by other threads)
//    - Mutexes ensure ATOMICITY (operations complete without interruption)
//
//    Example: clients[i].active = 1; clients[i].fd = fd;
//    Even with volatile, another thread might see active=1 but fd=-1 (halfway done)!
//    Mutexes guarantee all-or-nothing.
//
// 7. SYNCHRONIZATION PRIMITIVES:
//    - Mutex: One thread at a time can enter critical section
//    - Semaphore: N threads can enter (not shown here)
//    - Condition variable: Wait for a condition (not shown here)
//    - Read-write lock: Multiple readers OR one writer (not shown here)
//
// 8. PERFORMANCE CONSIDERATIONS:
//    Locks have overhead:
//    - Acquiring/releasing a lock takes time
//    - Contention (multiple threads wanting the lock) causes blocking
//    - Over-locking reduces parallelism
//    - Under-locking causes race conditions
//
//    This code prioritizes CORRECTNESS over maximum performance.
//
// 9. TESTING CONCURRENT CODE:
//    Race conditions are hard to test! They might only appear under high load
//    or on certain CPUs. Always use synchronization even if "it works on my machine."
//
// 10. ALTERNATIVES:
//    For very high concurrency, alternatives include:
//    - Lock-free data structures (atomic operations)
//    - Message passing (threads don't share data)
//    - Event-driven I/O (avoid threads entirely)
//
// ============================================================================
