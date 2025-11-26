// webserver_v2_commented.c
// Web server that serves static files from a directory.
// This demonstrates HTTP request parsing and file I/O integration.
//
// Compile: gcc -o webserver_v2_commented webserver_v2_commented.c
// Usage: ./webserver_v2_commented port webroot
// Example: ./webserver_v2_commented 8080 ./public
//
// Create a test file:
//   mkdir -p public
//   echo "<h1>Hello</h1>" > public/index.html
//   ./webserver_v2_commented 8080 public
//   Browse to http://localhost:8080/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>     // For stat() to get file information
#include <netinet/in.h>
#include <fcntl.h>        // For open()

#define BUFFER_SIZE 8192
#define MAX_PATH 512

// Global variable for the document root directory
// This is the base directory where files are served from
char *webroot;

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
    webroot = argv[2];  // Save the document root path

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

    printf("Web server running on http://localhost:%d\n", port);
    printf("Serving files from: %s\n", webroot);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        handle_client(client_fd);
        close(client_fd);
    }

    return 0;
}

// ============================================================================
// HTTP REQUEST HANDLER WITH PARSING
// ============================================================================
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return;
    buffer[bytes] = '\0';

    // ========================================================================
    // PARSE THE HTTP REQUEST LINE
    // ========================================================================
    // HTTP request first line format: METHOD PATH VERSION
    // Example: GET /index.html HTTP/1.1
    //
    // sscanf() is a simple way to parse this format.
    // More robust servers use specialized HTTP parser libraries.

    char method[16], path[MAX_PATH], version[16];

    // sscanf() with field widths (%15s, %511s) prevents buffer overflow
    // %15s reads at most 15 characters (leaving room for \0)
    if (sscanf(buffer, "%15s %511s %15s", method, path, version) != 3) {
        // Malformed request - couldn't parse all three fields
        send_error(client_fd, 400, "Bad Request");
        return;
    }

    // Log the request
    printf("%s %s %s\n", method, path, version);

    // ========================================================================
    // VALIDATE HTTP METHOD
    // ========================================================================
    // We only support GET requests
    // Real servers support GET, POST, PUT, DELETE, HEAD, OPTIONS, etc.

    if (strcmp(method, "GET") != 0) {
        send_error(client_fd, 405, "Method Not Allowed");
        return;
    }

    // ========================================================================
    // SECURITY: PREVENT DIRECTORY TRAVERSAL
    // ========================================================================
    // CRITICAL SECURITY CHECK!
    //
    // Attackers might try: GET /../../../etc/passwd HTTP/1.1
    // This would try to access files outside webroot!
    //
    // We prevent this by rejecting any path containing ".."
    // More sophisticated approaches:
    //   - Canonicalize the path (resolve all .. and .)
    //   - Use realpath() and verify result is under webroot
    //   - Chroot jail (advanced)

    if (strstr(path, "..") != NULL) {
        send_error(client_fd, 403, "Forbidden");
        return;
    }

    // ========================================================================
    // BUILD FULL FILE PATH
    // ========================================================================
    // Map URL path to filesystem path
    // Example: "/" -> "/home/user/public/index.html"
    //          "/style.css" -> "/home/user/public/style.css"

    char full_path[MAX_PATH];

    // Special case: "/" should serve index.html
    // This is a common web server convention
    if (strcmp(path, "/") == 0) {
        snprintf(full_path, sizeof(full_path), "%s/index.html", webroot);
    } else {
        // Concatenate webroot + path
        // Example: webroot="./public", path="/test.html" -> "./public/test.html"
        snprintf(full_path, sizeof(full_path), "%s%s", webroot, path);
    }

    // Try to serve the file
    send_file(client_fd, full_path);
}

// ============================================================================
// SEND FILE: Read from disk and send as HTTP response
// ============================================================================
void send_file(int client_fd, char *path) {
    // ========================================================================
    // OPEN THE FILE
    // ========================================================================
    // open() is a low-level file operation (vs fopen which is buffered)
    // O_RDONLY = open for reading only

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        // File doesn't exist or can't be read
        send_error(client_fd, 404, "Not Found");
        return;
    }

    // ========================================================================
    // GET FILE SIZE USING stat()
    // ========================================================================
    // stat() retrieves file metadata (size, permissions, timestamps, etc.)
    // fstat() is like stat() but works on an already-open file descriptor

    struct stat st;
    fstat(fd, &st);
    int file_size = st.st_size;

    // st.st_size is the file size in bytes
    // Other useful fields in struct stat:
    //   st.st_mode  - file permissions and type
    //   st.st_mtime - last modification time
    //   st.st_uid   - owner user ID

    // ========================================================================
    // READ ENTIRE FILE INTO MEMORY
    // ========================================================================
    // For simplicity, we read the entire file into a buffer.
    // This is fine for small files but TERRIBLE for large files!
    //
    // Better approaches for production:
    //   - sendfile() system call (zero-copy)
    //   - mmap() for memory-mapped files
    //   - Read and send in chunks
    //   - Use chunked transfer encoding

    char *body = malloc(file_size);
    if (body == NULL) {
        close(fd);
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }

    // read() from file descriptor
    // IMPORTANT: Like recv(), read() might not read all bytes in one call!
    // This code assumes it does (OK for small local files), but production
    // code should loop until all bytes are read.
    read(fd, body, file_size);
    close(fd);

    // ========================================================================
    // DETERMINE CONTENT TYPE
    // ========================================================================
    // The browser needs to know what kind of file this is
    // We determine this from the file extension

    char *content_type = get_content_type(path);

    // ========================================================================
    // SEND HTTP RESPONSE
    // ========================================================================
    send_response(client_fd, 200, "OK", content_type, body, file_size);

    // Clean up
    free(body);
}

// ============================================================================
// SEND HTTP RESPONSE
// ============================================================================
void send_response(int client_fd, int status, char *status_text,
                   char *content_type, char *body, int body_len) {
    // Build HTTP response headers
    char header[BUFFER_SIZE];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, status_text, content_type, body_len);

    // Send headers
    send(client_fd, header, header_len, 0);

    // Send body
    // NOTE: body might be binary data (images, PDFs, etc.), not just text!
    // That's why we use body_len instead of strlen()
    send(client_fd, body, body_len, 0);
}

// ============================================================================
// SEND ERROR RESPONSE
// ============================================================================
void send_error(int client_fd, int status, char *status_text) {
    // Generate a simple HTML error page
    char body[256];
    int body_len = snprintf(body, sizeof(body),
        "<html><body><h1>%d %s</h1></body></html>",
        status, status_text);

    send_response(client_fd, status, status_text, "text/html", body, body_len);
}

// ============================================================================
// DETERMINE CONTENT-TYPE FROM FILE EXTENSION
// ============================================================================
char *get_content_type(char *path) {
    // strrchr() finds the LAST occurrence of '.' in the path
    // This gives us the file extension
    char *ext = strrchr(path, '.');

    // No extension found
    if (ext == NULL) return "application/octet-stream";

    // Common file types
    // A real server would have a much longer list or use a MIME type database

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

    // Default for unknown types
    // "application/octet-stream" means "binary data, download it"
    return "application/octet-stream";
}

// ============================================================================
// KEY WEB SERVER CONCEPTS:
// ============================================================================
//
// 1. STATIC FILE SERVING:
//    This server serves files "as-is" from disk.
//    Contrast with dynamic content (PHP, Python, Node.js) which
//    generates responses programmatically.
//
// 2. DOCUMENT ROOT:
//    The base directory for files. All paths are relative to this.
//    Example: webroot="/var/www", path="/images/logo.png"
//             -> serves "/var/www/images/logo.png"
//
// 3. DIRECTORY TRAVERSAL ATTACKS:
//    CRITICAL SECURITY CONCERN!
//    Attackers try paths like: /../../../etc/passwd
//    Always validate paths! Never trust user input!
//
//    Defense layers:
//    - Reject paths with ".." (we do this)
//    - Canonicalize paths (realpath())
//    - Run server with minimal permissions
//    - Use chroot to jail the process
//
// 4. CONTENT-TYPE HEADER:
//    Tells browser how to handle the file:
//    - text/html -> render as HTML
//    - image/png -> display as image
//    - application/pdf -> open PDF viewer
//    - application/octet-stream -> download
//
//    Wrong content-type causes problems:
//    - Image served as text/html -> displayed as garbage text
//    - JavaScript served as text/plain -> not executed (security!)
//
// 5. FILE I/O:
//    open() / read() / close() vs fopen() / fread() / fclose()
//    - open/read/close: Low-level, unbuffered
//    - fopen/fread/fclose: Higher-level, buffered
//
//    For network servers, low-level is often preferred because
//    you have more control.
//
// 6. MEMORY MANAGEMENT:
//    We malloc() the file contents. This has problems:
//    - Large files consume lots of memory
//    - Multiple concurrent requests = lots of memory
//    - Risk of running out of memory
//
//    Better approaches:
//    - sendfile() system call (kernel does the copying)
//    - mmap() (memory-mapped files)
//    - Streaming (read chunks, send chunks)
//
// 7. HTTP STATUS CODES IN THIS SERVER:
//    200 OK - File found and sent
//    400 Bad Request - Malformed HTTP request
//    403 Forbidden - Path contains ".." (security)
//    404 Not Found - File doesn't exist
//    405 Method Not Allowed - Non-GET request
//    500 Internal Server Error - malloc failed
//
// 8. MISSING FEATURES (that real servers have):
//    - POST/PUT/DELETE support
//    - Request headers parsing (Host, User-Agent, etc.)
//    - Response headers (Cache-Control, ETag, Last-Modified)
//    - Range requests (for resuming downloads)
//    - Compression (gzip, brotli)
//    - Virtual hosts (multiple domains on one server)
//    - CGI/FastCGI (running programs to generate content)
//    - WebSocket support
//    - HTTPS/TLS encryption
//
// 9. MIME TYPES:
//    Standardized type/subtype format:
//    - text/plain, text/html, text/css
//    - image/jpeg, image/png, image/gif
//    - application/json, application/pdf
//    - video/mp4, audio/mp3
//
//    Browsers use this to know how to display content.
//    Servers typically have a MIME database (like /etc/mime.types).
//
// 10. INDEX FILES:
//    When requesting "/", we serve "index.html"
//    This is a common convention. Other common index files:
//    - index.htm
//    - default.html
//    - index.php
//
//    Real servers try multiple files in order.
//
// ============================================================================
