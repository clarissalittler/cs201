// echo_server_threaded_commented.c
// Multi-client echo server using POSIX threads (pthreads).
// This demonstrates how to handle multiple clients CONCURRENTLY,
// unlike the sequential echo_server which handles one client at a time.
//
// Compile: gcc -o echo_server_threaded_commented echo_server_threaded_commented.c -pthread
//          NOTE: The -pthread flag is REQUIRED when using pthreads!
// Usage: ./echo_server_threaded_commented port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>      // For pthread_create(), pthread_detach()

// Forward declaration of the client handler function
void *handle_client(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    // ========================================================================
    // SOCKET SETUP (same as echo_server.c)
    // ========================================================================
    // The socket creation, binding, and listening steps are identical to the
    // sequential server. The difference is in how we handle accepted connections.

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    // Enable SO_REUSEADDR to avoid "Address already in use" errors
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

    printf("Multi-client echo server listening on port %d...\n", port);

    // ========================================================================
    // CONCURRENT CONNECTION HANDLING WITH THREADS
    // ========================================================================
    // The main difference from the sequential server:
    // Instead of handling the client in the main thread (blocking other clients),
    // we create a NEW THREAD for each client.

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // accept() still blocks, but only until A client connects.
        // Once we accept a client, we immediately spawn a thread and
        // loop back to accept the next client.
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // ====================================================================
        // PASSING DATA TO THREADS: THE MALLOC IDIOM
        // ====================================================================
        // We need to pass client_fd to the new thread. Here's a common problem:
        //
        // WRONG APPROACH:
        //   pthread_create(&thread, NULL, handle_client, &client_fd);
        //
        // Why is this wrong? Because client_fd is a LOCAL VARIABLE in main().
        // By the time the thread runs, we might have looped back and called
        // accept() again, changing the value of client_fd!
        //
        // CORRECT APPROACH:
        // Allocate memory on the HEAP and pass a pointer to it.
        // The thread will free this memory when it's done.

        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        // WHY malloc()?
        // - Stack variables disappear when the function returns
        // - Each thread needs its own copy of the data
        // - Heap memory persists until explicitly freed

        // ====================================================================
        // CREATE A NEW THREAD
        // ====================================================================
        pthread_t thread;

        // pthread_create() creates a new thread of execution.
        // Parameters:
        //   &thread          - OUTPUT: thread identifier (like a process ID)
        //   NULL             - thread attributes (NULL = default)
        //   handle_client    - function to run in the new thread
        //   client_fd_ptr    - argument to pass to that function (void *)
        if (pthread_create(&thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(client_fd_ptr);  // Don't leak memory on error!
            continue;
        }

        // ====================================================================
        // DETACH THE THREAD
        // ====================================================================
        // pthread_detach() tells the system: "I don't need to join this thread later."
        //
        // BACKGROUND: Threads can be in two states:
        //   1. JOINABLE (default): Parent can call pthread_join() to wait for completion
        //                          and retrieve a return value. Thread resources aren't
        //                          fully freed until joined.
        //   2. DETACHED: Thread resources are automatically freed when it exits.
        //                You can't join it.
        //
        // For a server handling many clients, we don't care about return values
        // or waiting for threads to finish. We just want them to clean up automatically.
        // So we detach them.
        pthread_detach(thread);

        // The thread is now running handle_client() concurrently.
        // Meanwhile, the main thread loops back to accept() for the next client.
        // This is how we can handle MULTIPLE clients simultaneously!
    }

    return 0;
}

// ============================================================================
// CLIENT HANDLER THREAD
// ============================================================================
// This function runs in a SEPARATE THREAD for each connected client.
// Multiple instances of this function can run simultaneously!

void *handle_client(void *arg) {
    // ========================================================================
    // RETRIEVE THE CLIENT SOCKET
    // ========================================================================
    // arg is a void* (generic pointer). We cast it back to int* and dereference it.
    int client_fd = *(int *)arg;

    // Free the memory we allocated in main().
    // This thread now "owns" this memory, so it's responsible for freeing it.
    free(arg);

    // ========================================================================
    // ECHO LOOP (same as in echo_server.c)
    // ========================================================================
    // Each thread has its own stack, so buffer is THREAD-LOCAL.
    // Different threads can use the same variable names without conflicts.
    char buffer[1024];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        send(client_fd, buffer, bytes_received, 0);
    }

    // Client disconnected or error occurred.
    printf("Client disconnected.\n");

    // ========================================================================
    // CLEAN UP AND EXIT THREAD
    // ========================================================================
    close(client_fd);

    // Return NULL to indicate normal thread termination.
    // The return value would be available to pthread_join(), but since
    // this thread is detached, nobody will see this value.
    return NULL;

    // When this function returns, the thread automatically exits.
    // Because it's detached, all resources are automatically freed.
}

// ============================================================================
// KEY THREADING CONCEPTS DEMONSTRATED:
// ============================================================================
//
// 1. CONCURRENCY VS PARALLELISM:
//    - Concurrency: Multiple tasks in progress (threads can interleave)
//    - Parallelism: Multiple tasks running simultaneously (on multiple CPU cores)
//    Threads give us concurrency. On a multi-core CPU, we also get parallelism.
//
// 2. THREAD CREATION COST:
//    Creating a thread is cheaper than creating a process (fork()), but not free.
//    For very high connection counts, thread pools or event-driven I/O (epoll)
//    are more scalable. But for moderate loads, threads are simple and effective.
//
// 3. THREAD SAFETY:
//    Each thread has its own STACK (local variables, function call stack).
//    They share the HEAP, global variables, and file descriptors.
//
//    In this example, we're safe because:
//    - Each thread has its own client_fd (different socket)
//    - Each thread has its own buffer (on its stack)
//    - We're not sharing any data between threads
//
//    If threads DID share data, we'd need mutexes (see chat_server.c).
//
// 4. MALLOC FOR THREAD ARGUMENTS:
//    This is a standard idiom: allocate arguments on the heap, pass to thread,
//    thread frees them. Ensures data persists for the thread's lifetime.
//
// 5. DETACHED THREADS:
//    Servers typically create many short-lived threads. Detaching prevents
//    resource leaks without requiring manual joins.
//
// 6. FILE DESCRIPTOR BEHAVIOR:
//    File descriptors are PROCESS-WIDE resources (shared by all threads).
//    - Multiple threads CAN use different file descriptors safely (like we do)
//    - Multiple threads should NOT use the SAME file descriptor without synchronization
//    - close() in one thread affects all threads (another reason each thread has its own client_fd)
//
// 7. COMPILATION:
//    The -pthread flag does three things:
//    - Links the pthread library
//    - Defines _REENTRANT (makes standard library functions thread-safe)
//    - Sets compiler flags for thread-safe code generation
//
// 8. SCALABILITY:
//    This approach scales well to dozens or hundreds of concurrent clients.
//    For thousands+ clients, consider:
//    - Thread pools (reuse threads instead of creating/destroying)
//    - Event-driven I/O (select/poll/epoll - one thread handles many sockets)
//
// 9. ERROR HANDLING:
//    Notice we don't check send() return value in the thread. In production,
//    you'd want to handle partial sends and errors more carefully.
//
// ============================================================================
