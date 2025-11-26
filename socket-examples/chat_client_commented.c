// chat_client_commented.c
// Chat client with BIDIRECTIONAL communication using threads.
// This demonstrates how to send AND receive simultaneously, which is essential
// for interactive chat applications.
//
// Compile: gcc -o chat_client_commented chat_client_commented.c -pthread
// Usage: ./chat_client_commented hostname port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

// ============================================================================
// GLOBAL VARIABLES FOR THREAD COMMUNICATION
// ============================================================================
// Threads share the process's global variables. We use these to coordinate
// between the main thread (sending) and the receive thread.

int sockfd;  // The socket file descriptor, shared by both threads

// volatile tells the compiler: "This variable can change unexpectedly."
// Without it, the compiler might optimize away checks of this variable
// (e.g., caching it in a register). With it, the compiler always reads
// from memory, ensuring threads see each other's changes.
//
// WHY volatile? The receive thread might set running=0, and we need the
// main thread's loop to see that change immediately.
volatile int running = 1;

// Forward declaration
void *receive_thread(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    char *hostname = argv[1];
    char *port = argv[2];

    // ========================================================================
    // CONNECT TO SERVER (same as tcp_client.c)
    // ========================================================================
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    freeaddrinfo(res);
    printf("Connected to %s:%s\n", hostname, port);
    printf("Type messages and press Enter. Ctrl+C to quit.\n\n");

    // ========================================================================
    // CREATE A SEPARATE THREAD FOR RECEIVING
    // ========================================================================
    // THE PROBLEM: We need to do two things simultaneously:
    //   1. Read from stdin and send to server (blocking on fgets)
    //   2. Receive from server and print (blocking on recv)
    //
    // If we do both in one thread, we'd block on one and miss the other!
    //
    // THE SOLUTION: Two threads
    //   - Main thread: handles sending (reads stdin, sends to socket)
    //   - Receive thread: handles receiving (reads socket, prints to stdout)

    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receive_thread, NULL) != 0) {
        perror("pthread_create");
        close(sockfd);
        exit(1);
    }

    // NOTE: We're passing NULL as the argument because the receive thread
    // will use the global variable sockfd. Both threads share the same
    // socket descriptor.

    // ========================================================================
    // MAIN THREAD: SENDING LOOP
    // ========================================================================
    // The main thread handles user input and sending.
    // This runs concurrently with the receive thread.

    char buffer[BUFFER_SIZE];

    // Loop while:
    //   - running is true (set to false if server disconnects)
    //   - fgets succeeds (returns non-NULL)
    while (running && fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // fgets() blocks waiting for user input. That's OK because the
        // receive thread is independently handling incoming messages.

        // fgets() reads a line including the newline character.
        // We send it as-is (the server can handle it).

        if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            break;  // Exit on send error
        }
    }

    // ========================================================================
    // CLEANUP
    // ========================================================================
    // We've exited the sending loop. Either:
    //   1. User pressed Ctrl+D (EOF on stdin)
    //   2. send() failed
    //   3. Receive thread set running=0 (server disconnected)

    running = 0;  // Signal the receive thread to stop

    // Close the socket. This will cause the receive thread's recv() to return
    // (either with 0 for clean shutdown or -1 for error), allowing it to exit.
    close(sockfd);

    // ====================================================================
    // THREAD CLEANUP: CANCEL AND JOIN
    // ====================================================================
    // pthread_cancel() requests that the thread terminate.
    // This is used when the thread might be blocked in a system call (like recv).
    pthread_cancel(recv_thread);

    // pthread_join() waits for the thread to actually terminate.
    // Parameters:
    //   recv_thread - the thread to wait for
    //   NULL        - we don't care about the return value
    //
    // WHY join instead of detach?
    // Because we want to ensure the thread is fully cleaned up before our
    // process exits. Detached threads might still be running when main() exits.
    pthread_join(recv_thread, NULL);

    printf("\nDisconnected.\n");
    return 0;
}

// ============================================================================
// RECEIVE THREAD
// ============================================================================
// This function runs in a separate thread, continuously receiving messages
// from the server and printing them.

void *receive_thread(void *arg) {
    (void)arg;  // Suppress "unused parameter" warning

    char buffer[BUFFER_SIZE];
    ssize_t bytes;

    // Loop while:
    //   - running is true (main thread might set to false)
    //   - recv() returns > 0 (received data)
    while (running && (bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        // We received data from the server!

        // Null-terminate so we can print it as a string.
        // This assumes the server is sending text. For binary data, you'd
        // handle it differently.
        buffer[bytes] = '\0';

        // Print the message.
        // NOTE: We're using printf from a different thread than main().
        // This is generally safe for stdout, but can cause interleaved output.
        // For production, you might use a mutex or a dedicated output thread.
        printf("%s", buffer);

        // fflush ensures the output is displayed immediately, even if it
        // doesn't end with a newline. Without this, output might be buffered
        // and delayed (especially if stdout is redirected).
        fflush(stdout);
    }

    // We've exited the loop. Either:
    //   1. recv() returned 0 (server closed connection)
    //   2. recv() returned -1 (error)
    //   3. running became false (main thread signaled shutdown)

    if (bytes == 0 && running) {
        // Server closed the connection while we thought we were still running.
        printf("\nServer closed connection.\n");
        running = 0;  // Signal the main thread to stop
    }

    return NULL;
}

// ============================================================================
// KEY CONCEPTS DEMONSTRATED:
// ============================================================================
//
// 1. BIDIRECTIONAL COMMUNICATION:
//    This is the pattern for any interactive network application:
//    - One thread for sending (often reading from user input)
//    - One thread for receiving (often displaying to user)
//    Without threads, you'd need non-blocking I/O and select/poll/epoll.
//
// 2. SHARED SOCKET:
//    Both threads use the same socket descriptor (sockfd).
//    This is safe because:
//    - One thread only calls send()
//    - One thread only calls recv()
//    - These don't interfere with each other (kernel keeps separate buffers)
//
//    UNSAFE would be: both threads calling send() simultaneously, or both
//    calling recv(). That requires synchronization (mutexes).
//
// 3. GLOBAL VARIABLES VS THREAD-LOCAL:
//    - sockfd and running are GLOBAL (shared by threads)
//    - buffer is declared in each function, so each thread has its OWN copy
//    - This is because each thread has its own stack
//
// 4. VOLATILE KEYWORD:
//    Tells compiler this variable can change unexpectedly (by another thread).
//    Without volatile, optimizations might cache the value and miss updates.
//    Essential for inter-thread flags!
//
// 5. THREAD CANCELLATION:
//    pthread_cancel() is used when a thread might be blocked in a system call.
//    It's a somewhat blunt instrument - clean shutdown is often better.
//    In this case, close(sockfd) would also wake up the recv().
//
// 6. JOINING VS DETACHING:
//    Here we join because we want orderly shutdown. If we detached and
//    immediately exited main(), the receive thread might still be printing,
//    which could cause issues.
//
// 7. RACE CONDITIONS:
//    There are subtle race conditions in this code!
//    - Both threads might set running=0 simultaneously (harmless)
//    - Both threads might check running simultaneously (harmless)
//    - But for more complex shared state, you'd need mutexes
//
//    This code works because:
//    - running is only changed to 0, never back to 1
//    - It's volatile, so changes are visible
//    - We're okay with either thread seeing the change slightly late
//
// 8. OUTPUT INTERLEAVING:
//    Because both threads might print (main prints "Sent", receive prints
//    messages), output can interleave. Try running this and typing fast!
//    Production code might use a message queue and dedicated output thread.
//
// 9. ERROR HANDLING:
//    This example assumes text messages. For binary protocols, you'd need
//    length prefixes or delimiters to know where messages start/end.
//
// 10. BLOCKING I/O:
//    Both fgets() and recv() are blocking calls. That's OK because each
//    has its own thread! This is why threads are useful - they make blocking
//    I/O code simple and readable.
//
// ============================================================================
