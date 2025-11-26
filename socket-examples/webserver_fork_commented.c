// webserver_fork_commented.c
// Multi-client web server using fork() for process-based concurrency.
// This demonstrates an alternative to threading: creating separate processes
// to handle each request.
//
// Compile: gcc -o webserver_fork_commented webserver_fork_commented.c
// Usage: ./webserver_fork_commented port webroot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>     // For waitpid() to handle child processes
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>       // For signal handling

#define BUFFER_SIZE 8192
#define MAX_PATH 512

char *webroot;

void handle_client(int client_fd);
void send_response(int client_fd, int status, char *status_text,
                   char *content_type, char *body, int body_len);
void send_file(int client_fd, char *path);
void send_error(int client_fd, int status, char *status_text);
char *get_content_type(char *path);
void sigchld_handler(int sig);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s port webroot\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    webroot = argv[2];

    // ========================================================================
    // SIGNAL HANDLER FOR REAPING ZOMBIE PROCESSES
    // ========================================================================
    // THE ZOMBIE PROBLEM:
    // When a child process exits, it becomes a "zombie" - it's dead but its
    // entry in the process table remains. The parent must "reap" it (collect
    // its exit status) with wait() or waitpid().
    //
    // If we don't reap zombies, they accumulate and eventually we run out
    // of process table entries!
    //
    // THE SOLUTION:
    // Register a signal handler for SIGCHLD. When a child exits, the kernel
    // sends SIGCHLD to the parent. Our handler reaps all dead children.

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;  // Our handler function

    // sigemptyset() initializes the signal mask to empty
    // (no signals are blocked during handler execution)
    sigemptyset(&sa.sa_mask);

    // SA_RESTART: Automatically restart system calls interrupted by signals
    // Without this, accept() might return EINTR when SIGCHLD arrives
    sa.sa_flags = SA_RESTART;

    // sigaction() registers the handler
    // Parameters:
    //   SIGCHLD - signal to handle (sent when child process changes state)
    //   &sa     - new action to take
    //   NULL    - we don't need the old action
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // ========================================================================
    // SOCKET SETUP (same as before)
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

    printf("Web server (fork) running on http://localhost:%d\n", port);
    printf("Serving files from: %s\n", webroot);

    // ========================================================================
    // MAIN ACCEPT LOOP WITH fork()
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
        // CREATE A NEW PROCESS WITH fork()
        // ====================================================================
        // fork() creates a COPY of the current process
        // Returns:
        //   -1: Error
        //   0:  In the child process
        //   >0: In the parent process (the value is the child's PID)

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            close(client_fd);
            continue;
        }

        // ====================================================================
        // CHILD PROCESS PATH
        // ====================================================================
        if (pid == 0) {
            // We are in the CHILD process!
            // The child is a complete copy of the parent, including:
            //   - Memory (copy-on-write)
            //   - File descriptors
            //   - Signal handlers
            //   - Current directory
            // But it has a different PID.

            // The child doesn't need the server socket (it's not listening)
            close(server_fd);

            // Handle this client
            handle_client(client_fd);

            // Close the client socket
            close(client_fd);

            // CRITICAL: Child must exit!
            // If we don't exit, the child would loop back and call accept()!
            // Then we'd have multiple processes all accepting connections.
            exit(0);
        }

        // ====================================================================
        // PARENT PROCESS PATH
        // ====================================================================
        else {
            // We are in the PARENT process
            // The parent continues to accept new connections

            // The parent doesn't need this particular client's socket
            // The child has its own copy, so we close ours
            close(client_fd);

            // The parent immediately loops back to accept() the next connection
            // Meanwhile, the child is independently handling this request
        }
    }

    return 0;
}

// ============================================================================
// SIGNAL HANDLER: Reap zombie children
// ============================================================================
void sigchld_handler(int sig) {
    (void)sig;  // Unused parameter

    // waitpid() reaps (collects) terminated child processes
    // Parameters:
    //   -1         - wait for ANY child process
    //   NULL       - we don't care about exit status
    //   WNOHANG    - don't block if no child has exited
    //
    // Returns:
    //   >0: PID of reaped child
    //   0:  No children ready (WNOHANG)
    //   -1: Error or no children

    // Loop while there are children to reap
    // Multiple children might exit around the same time, so we keep calling
    // until there are no more zombies
    while (waitpid(-1, NULL, WNOHANG) > 0);

    // IMPORTANT: Signal handlers should be BRIEF and simple
    // - Don't call non-async-signal-safe functions (printf, malloc, etc.)
    // - Don't do complex logic
    // - Just do the minimum necessary and return
    //
    // waitpid() is async-signal-safe, so it's OK to call here
}

// ============================================================================
// HANDLE_CLIENT and helper functions (same as webserver_v2.c)
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

    // Log with PID to show which process handled this request
    printf("[PID %d] %s %s %s\n", getpid(), method, path, version);

    if (strcmp(method, "GET") != 0) {
        send_error(client_fd, 405, "Method Not Allowed");
        return;
    }

    if (strstr(path, "..") != NULL) {
        send_error(client_fd, 403, "Forbidden");
        return;
    }

    char full_path[MAX_PATH];
    if (strcmp(path, "/") == 0) {
        snprintf(full_path, sizeof(full_path), "%s/index.html", webroot);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", webroot, path);
    }

    send_file(client_fd, full_path);
}

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

void send_error(int client_fd, int status, char *status_text) {
    char body[256];
    int body_len = snprintf(body, sizeof(body),
        "<html><body><h1>%d %s</h1></body></html>",
        status, status_text);
    send_response(client_fd, status, status_text, "text/html", body, body_len);
}

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
// FORK VS THREADS: KEY DIFFERENCES
// ============================================================================
//
// 1. PROCESS ISOLATION:
//    fork(): Each child is a SEPARATE PROCESS with its own memory space
//    threads: All threads share the SAME memory space
//
//    fork() pros:
//    - True isolation - child crash doesn't affect parent
//    - No race conditions on memory (each has own copy)
//    - Security - easier to sandbox children
//
//    fork() cons:
//    - Higher memory usage (each process has its own memory)
//    - Slower to create (must copy process state)
//    - Can't easily share data between processes
//
// 2. COPY-ON-WRITE (COW):
//    Modern Unix systems optimize fork() with COW:
//    - Initially, child shares parent's memory pages (read-only)
//    - When either writes to a page, it's copied
//    - Result: fork() is fast, memory efficient if no writes
//
// 3. FILE DESCRIPTORS:
//    Both processes have copies of all file descriptors
//    They refer to the SAME underlying kernel objects
//    That's why we must:
//    - Child closes server_fd (doesn't need it)
//    - Parent closes client_fd (child has it)
//    Otherwise: resource leaks!
//
// 4. ZOMBIE PROCESSES:
//    When a child exits, it becomes a zombie until parent reaps it
//    Must use wait()/waitpid() or signal handler
//    Failure to reap -> process table fills up!
//
// 5. SIGNALS:
//    SIGCHLD: Sent to parent when child changes state (exits, stops, etc.)
//    SA_RESTART: Restart interrupted system calls automatically
//    async-signal-safe: Only certain functions safe to call in handlers
//
// 6. HISTORICAL CONTEXT:
//    fork() is the traditional Unix concurrency model
//    Apache HTTP Server used "prefork" model (fork per connection)
//    Now threads and event-driven I/O are more common
//    But fork() is still used for:
//    - CGI scripts
//    - Privilege separation
//    - Task isolation
//
// 7. PERFORMANCE CHARACTERISTICS:
//    fork() overhead: ~100-200 microseconds (process creation)
//    pthread overhead: ~10-20 microseconds (thread creation)
//
//    For high-traffic servers: threads or event-driven I/O are better
//    For security-critical: fork() provides better isolation
//
// 8. SHARED STATE:
//    fork(): No shared state (copy-on-write)
//    threads: Must use mutexes for shared state
//
//    Our fork()-based server doesn't need mutexes!
//    Each child process is independent.
//
// 9. RESOURCE LIMITS:
//    Systems have limits on:
//    - Max processes per user
//    - Max processes system-wide
//    - Max threads per process
//
//    Check with: ulimit -u (max user processes)
//
// 10. WHEN TO USE FORK():
//    - Need true isolation (crash containment)
//    - Running untrusted code
//    - Don't need to share state
//    - Can tolerate higher memory usage
//    - Security is paramount
//
//    WHEN TO USE THREADS:
//    - Need to share state
//    - Want lower overhead
//    - High connection count
//    - Limited memory
//
// ============================================================================
