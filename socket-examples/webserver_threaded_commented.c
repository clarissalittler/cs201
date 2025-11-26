// webserver_threaded_commented.c
// Multi-client web server using POSIX threads (pthreads).
// This is the thread-based equivalent of webserver_fork.c, demonstrating
// thread-based concurrency for handling multiple HTTP requests simultaneously.
//
// Compile: gcc -o webserver_threaded_commented webserver_threaded_commented.c -pthread
// Usage: ./webserver_threaded_commented port webroot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFFER_SIZE 8192
#define MAX_PATH 512

char *webroot;

void *client_thread(void *arg);
void handle_client(int client_fd);
void send_response(int client_fd, int status, char *status_text,
                   char *content_type, char *body, int body_len);
void send_file(int client_fd, char *path);
void send_error(int client_fd, int status, char *status_text);
char *get_content_type(char *path);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s port webroot\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    webroot = argv[2];

    // ========================================================================
    // SOCKET SETUP
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

    printf("Web server (threaded) running on http://localhost:%d\n", port);
    printf("Serving files from: %s\n", webroot);

    // ========================================================================
    // MAIN ACCEPT LOOP WITH THREAD CREATION
    // ========================================================================
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        // ====================================================================
        // ALLOCATE AND PASS CLIENT FD TO THREAD
        // ====================================================================
        // As discussed in echo_server_threaded.c, we must allocate the client_fd
        // on the heap because local variables disappear when we loop back to accept().

        int *client_fd_ptr = malloc(sizeof(int));
        if (client_fd_ptr == NULL) {
            perror("malloc");
            close(client_fd);
            continue;
        }
        *client_fd_ptr = client_fd;

        // ====================================================================
        // CREATE A NEW THREAD
        // ====================================================================
        pthread_t thread;

        if (pthread_create(&thread, NULL, client_thread, client_fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(client_fd_ptr);
            continue;
        }

        // ====================================================================
        // DETACH THE THREAD
        // ====================================================================
        // Detaching allows the thread to clean up automatically when it exits.
        // We don't need to join it or get its return value.
        pthread_detach(thread);

        // Main thread immediately loops back to accept the next connection
        // Meanwhile, the new thread is handling this request concurrently
    }

    return 0;
}

// ============================================================================
// THREAD ENTRY POINT
// ============================================================================
// This function runs in a separate thread for each HTTP request.
// Multiple instances can run simultaneously.

void *client_thread(void *arg) {
    // Retrieve and free the client_fd we allocated in main()
    int client_fd = *(int *)arg;
    free(arg);

    // Handle this client's request
    handle_client(client_fd);

    // Close the socket when done
    close(client_fd);

    // Thread exits, and because it's detached, all resources are freed
    return NULL;
}

// ============================================================================
// HTTP REQUEST HANDLER
// ============================================================================
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return;
    buffer[bytes] = '\0';

    char method[16], path[MAX_PATH], version[16];
    if (sscanf(buffer, "%15s %511s %15s", method, path, version) != 3) {
        send_error(client_fd, 400, "Bad Request");
        return;
    }

    // Log request with thread ID
    // pthread_self() returns the current thread's ID (a pthread_t)
    // We cast to unsigned long for printing
    printf("[Thread %lu] %s %s %s\n", (unsigned long)pthread_self(), method, path, version);

    if (strcmp(method, "GET") != 0) {
        send_error(client_fd, 405, "Method Not Allowed");
        return;
    }

    // Security check: prevent directory traversal
    if (strstr(path, "..") != NULL) {
        send_error(client_fd, 403, "Forbidden");
        return;
    }

    // Build full file path
    char full_path[MAX_PATH];
    if (strcmp(path, "/") == 0) {
        snprintf(full_path, sizeof(full_path), "%s/index.html", webroot);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", webroot, path);
    }

    send_file(client_fd, full_path);
}

// ============================================================================
// FILE SERVING
// ============================================================================
void send_file(int client_fd, char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        send_error(client_fd, 404, "Not Found");
        return;
    }

    struct stat st;
    fstat(fd, &st);
    int file_size = st.st_size;

    char *body = malloc(file_size);
    if (body == NULL) {
        close(fd);
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }
    read(fd, body, file_size);
    close(fd);

    char *content_type = get_content_type(path);
    send_response(client_fd, 200, "OK", content_type, body, file_size);

    free(body);
}

// ============================================================================
// HTTP RESPONSE CONSTRUCTION
// ============================================================================
void send_response(int client_fd, int status, char *status_text,
                   char *content_type, char *body, int body_len) {
    char header[BUFFER_SIZE];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, status_text, content_type, body_len);

    send(client_fd, header, header_len, 0);
    send(client_fd, body, body_len, 0);
}

// ============================================================================
// ERROR RESPONSE
// ============================================================================
void send_error(int client_fd, int status, char *status_text) {
    char body[256];
    int body_len = snprintf(body, sizeof(body),
        "<html><body><h1>%d %s</h1></body></html>",
        status, status_text);
    send_response(client_fd, status, status_text, "text/html", body, body_len);
}

// ============================================================================
// MIME TYPE DETECTION
// ============================================================================
char *get_content_type(char *path) {
    char *ext = strrchr(path, '.');
    if (ext == NULL) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
        return "text/html";
    if (strcmp(ext, ".css") == 0)
        return "text/css";
    if (strcmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcmp(ext, ".png") == 0)
        return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(ext, ".gif") == 0)
        return "image/gif";
    if (strcmp(ext, ".txt") == 0)
        return "text/plain";

    return "application/octet-stream";
}

// ============================================================================
// THREADS VS FORK VS EVENT-DRIVEN: COMPARISON
// ============================================================================
//
// 1. THREAD-BASED (this server):
//    Pros:
//    - Lower overhead than fork() (~10-20μs vs ~100-200μs)
//    - Shared memory makes data sharing easy (when needed)
//    - Efficient for moderate concurrency (dozens to hundreds of connections)
//
//    Cons:
//    - Shared memory requires careful synchronization (mutexes)
//    - One thread crash can corrupt the whole process
//    - Thread limits (typically a few thousand)
//    - Potential race conditions
//
// 2. FORK-BASED (webserver_fork.c):
//    Pros:
//    - True isolation (child crash doesn't affect others)
//    - No race conditions on memory (separate address spaces)
//    - Better security (can drop privileges per-child)
//
//    Cons:
//    - Higher overhead (process creation)
//    - More memory usage (separate address spaces)
//    - Harder to share data between processes
//    - Need to handle zombies (SIGCHLD)
//
// 3. EVENT-DRIVEN (select/poll/epoll):
//    Pros:
//    - Can handle thousands+ connections
//    - Very low overhead per connection
//    - Single-threaded = no race conditions
//    - Efficient use of resources
//
//    Cons:
//    - More complex code (state machines)
//    - Blocking operations block everything
//    - Harder to understand and debug
//    - Not suitable for CPU-intensive work
//
// ============================================================================
// THREAD SAFETY IN THIS SERVER
// ============================================================================
//
// IS THIS SERVER THREAD-SAFE?
// Mostly yes, because:
//   1. Each thread has its own client_fd (no sharing)
//   2. Each thread has its own stack variables (buffer, path, etc.)
//   3. webroot is read-only after initialization (safe to share)
//
// POTENTIAL RACE CONDITIONS:
//   1. printf() - threads might interleave output
//      - Minor issue, just cosmetic
//      - Could use mutex if we care
//
//   2. Global variables - webroot is read-only (safe)
//      - If it were writeable, we'd need a mutex
//
//   3. malloc/free - standard library is thread-safe
//      - Modern libc implementations use internal locks
//
//   4. File descriptors - each thread has unique client_fd
//      - No conflicts possible
//
// WHY NO MUTEXES?
//   Unlike chat_server.c which shared a client list, this server's threads
//   are completely independent. They don't share any writeable data.
//
// ============================================================================
// SCALABILITY CONSIDERATIONS
// ============================================================================
//
// HOW MANY CONCURRENT CONNECTIONS CAN THIS HANDLE?
//
// Limiting factors:
//   1. Thread creation overhead
//      - Creating 1000 threads/sec = ~10-20ms overhead
//      - Acceptable for many workloads
//
//   2. Memory per thread
//      - Each thread has a stack (typically 2-8MB)
//      - 1000 threads = 2-8GB just for stacks!
//      - Can reduce with pthread_attr_setstacksize()
//
//   3. Context switching
//      - More threads = more context switches
//      - CPU time wasted switching between threads
//
//   4. System limits
//      - Max threads per process (check: ulimit -u)
//      - Typically a few thousand
//
// PERFORMANCE OPTIMIZATIONS:
//   1. Thread pool
//      - Pre-create threads, reuse them
//      - Avoids creation overhead
//
//   2. Smaller stack size
//      - Default is often larger than needed
//      - Use pthread_attr_setstacksize()
//
//   3. sendfile() system call
//      - Zero-copy file transmission
//      - Kernel does the work, very efficient
//
//   4. HTTP keep-alive
//      - Reuse connections for multiple requests
//      - Reduces connection overhead
//
//   5. Hybrid approach
//      - Thread pool + epoll
//      - Best of both worlds
//
// ============================================================================
// REAL-WORLD WEB SERVER ARCHITECTURES
// ============================================================================
//
// 1. APACHE (prefork):
//    - fork() per connection (like webserver_fork.c)
//    - Very stable, good isolation
//    - High memory usage
//
// 2. APACHE (worker):
//    - Threads per process
//    - Better than prefork for memory
//    - Still not as efficient as event-driven
//
// 3. NGINX:
//    - Event-driven (epoll)
//    - Can handle 10,000+ connections
//    - Low memory footprint
//    - Industry standard for high-traffic sites
//
// 4. NODE.JS:
//    - Single-threaded event loop
//    - Asynchronous I/O
//    - Good for I/O-bound workloads
//
// 5. GO HTTP SERVER:
//    - Goroutines (lightweight threads)
//    - Thousands of goroutines are cheap
//    - Runtime handles scheduling
//
// 6. THREAD POOL SERVERS:
//    - Pre-created worker threads
//    - Work queue
//    - Good balance of simplicity and performance
//
// ============================================================================
// PRODUCTION CONSIDERATIONS NOT IN THIS EXAMPLE
// ============================================================================
//
// 1. REQUEST LIMITS:
//    - Limit request size (prevent memory exhaustion)
//    - Timeout long connections
//    - Rate limiting per IP
//
// 2. ERROR HANDLING:
//    - Robust parsing (don't crash on malformed requests)
//    - Graceful degradation
//    - Proper logging
//
// 3. SECURITY:
//    - TLS/SSL encryption
//    - Input validation
//    - Buffer overflow prevention
//    - DoS protection
//
// 4. PERFORMANCE:
//    - Caching (ETag, Last-Modified headers)
//    - Compression (gzip, brotli)
//    - HTTP/2 multiplexing
//    - CDN integration
//
// 5. RELIABILITY:
//    - Graceful shutdown
//    - Request timeout
//    - Resource limits
//    - Health checks
//
// 6. MONITORING:
//    - Request logging
//    - Performance metrics
//    - Error tracking
//    - Alerting
//
// ============================================================================
