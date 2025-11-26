// webserver_v1_commented.c
// Minimal HTTP/1.1 web server with hardcoded response.
// This demonstrates the basics of the HTTP protocol over TCP sockets.
//
// Compile: gcc -o webserver_v1_commented webserver_v1_commented.c
// Usage: ./webserver_v1_commented port
// Test: Open a browser to http://localhost:PORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 4096

void handle_client(int client_fd);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

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

    printf("Web server running on http://localhost:%d\n", port);

    // ========================================================================
    // SEQUENTIAL REQUEST HANDLING
    // ========================================================================
    // This is a simple, sequential server like our first echo_server.
    // It handles one request at a time. This is fine for development and
    // learning, but production servers need concurrency (threads, fork, or epoll).

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        // Handle this request
        handle_client(client_fd);

        // Close the connection immediately after responding
        // This is HTTP/1.0 style (one request per connection)
        // HTTP/1.1 supports "keep-alive" to reuse connections, but we don't implement that
        close(client_fd);
    }

    return 0;
}

// ============================================================================
// HTTP REQUEST HANDLER
// ============================================================================
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    // ========================================================================
    // RECEIVE THE HTTP REQUEST
    // ========================================================================
    // HTTP requests are text-based and look like this:
    //
    //   GET /index.html HTTP/1.1
    //   Host: localhost:8080
    //   User-Agent: Mozilla/5.0...
    //   Accept: text/html...
    //   <blank line>
    //
    // The first line is the "request line"
    // Following lines are "headers"
    // A blank line (\r\n\r\n) marks the end of headers

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return;

    // Null-terminate so we can print/parse it as a string
    buffer[bytes] = '\0';

    // Print the request for debugging
    // In production, you'd use proper logging, not printf
    printf("--- Request ---\n%s\n", buffer);

    // ========================================================================
    // HTTP REQUEST PARSING (MINIMAL)
    // ========================================================================
    // For this simple server, we IGNORE the request!
    // We always return the same response regardless of what was requested.
    // This demonstrates that HTTP is just a protocol on top of TCP.
    //
    // Real servers parse:
    // - Method (GET, POST, PUT, DELETE, etc.)
    // - Path (/index.html, /api/users, etc.)
    // - Version (HTTP/1.0, HTTP/1.1, HTTP/2)
    // - Headers (Host:, Content-Type:, etc.)
    // - Body (for POST/PUT requests)

    // ========================================================================
    // PREPARE THE HTTP RESPONSE
    // ========================================================================
    // HTTP responses have this structure:
    //
    //   HTTP/1.1 200 OK\r\n              <- Status line
    //   Content-Type: text/html\r\n      <- Headers
    //   Content-Length: 52\r\n
    //   Connection: close\r\n
    //   \r\n                              <- Blank line separates headers from body
    //   <html><body><h1>Hello from C!</h1></body></html>  <- Body

    // The body (HTML content)
    char *body = "<html><body><h1>Hello from C!</h1></body></html>";
    int body_len = strlen(body);

    // Build the complete response (headers + body)
    char response[BUFFER_SIZE];
    int response_len = snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"                    // Status line
        "Content-Type: text/html\r\n"            // What kind of data we're sending
        "Content-Length: %d\r\n"                 // How many bytes in the body
        "Connection: close\r\n"                  // Close connection after this response
        "\r\n"                                   // Blank line (REQUIRED!)
        "%s",                                    // The actual body
        body_len, body);

    // ========================================================================
    // HTTP STATUS CODES
    // ========================================================================
    // The "200 OK" is a status code. Common ones:
    //   200 OK                  - Success
    //   404 Not Found           - Requested resource doesn't exist
    //   500 Internal Server Error - Server crashed/bug
    //   301 Moved Permanently   - Resource is at a different URL
    //   403 Forbidden           - Not allowed to access this
    //
    // These are standardized so browsers know how to handle them.

    // ========================================================================
    // HTTP HEADERS EXPLAINED
    // ========================================================================
    // Content-Type: Tells the browser what kind of data this is
    //   - text/html for HTML pages
    //   - application/json for JSON data
    //   - image/png for PNG images
    //   - etc.
    //
    // Content-Length: How many bytes are in the body
    //   - Lets the browser know when it has received the complete response
    //   - Required for HTTP/1.1 if not using chunked encoding
    //
    // Connection: close
    //   - Tells browser we'll close the connection after this response
    //   - HTTP/1.1 defaults to "keep-alive" (reuse connection for multiple requests)
    //   - We use "close" to keep the server simple

    // ========================================================================
    // LINE ENDINGS: \r\n
    // ========================================================================
    // HTTP requires CRLF (Carriage Return + Line Feed) = \r\n
    // This is different from Unix text files which use just \n
    // Historical reasons: teletype machines needed both characters
    //
    // IMPORTANT: The blank line between headers and body MUST be \r\n\r\n
    // A browser won't recognize \n\n - it must be \r\n\r\n

    // ========================================================================
    // SEND THE RESPONSE
    // ========================================================================
    send(client_fd, response, response_len, 0);

    // NOTE: We're not checking if all bytes were sent!
    // Production code should loop to ensure complete transmission.
    // For small responses over loopback, this usually works fine.
}

// ============================================================================
// KEY HTTP CONCEPTS:
// ============================================================================
//
// 1. HTTP IS TEXT-BASED:
//    Unlike binary protocols, HTTP/1.1 uses human-readable text.
//    You can literally type HTTP requests in telnet!
//    Example:
//      telnet localhost 8080
//      GET / HTTP/1.1
//      Host: localhost
//      <press Enter twice>
//
// 2. HTTP RUNS ON TOP OF TCP:
//    HTTP doesn't create its own connections. It uses TCP sockets.
//    All the socket code (bind, listen, accept) is the same.
//    HTTP is just a convention for what text to send over those sockets.
//
// 3. REQUEST-RESPONSE MODEL:
//    - Client sends a request
//    - Server sends ONE response
//    - Connection closes (in HTTP/1.0 style) or stays open (HTTP/1.1 keep-alive)
//
// 4. STATELESS PROTOCOL:
//    HTTP itself has no memory of previous requests.
//    Each request is independent. State is maintained via:
//    - Cookies
//    - Session tokens
//    - Database lookups
//
// 5. METHODS (VERBS):
//    - GET: Retrieve data (should not modify server state)
//    - POST: Submit data (often creates something)
//    - PUT: Update data
//    - DELETE: Delete data
//    - HEAD: Like GET but only returns headers
//    - OPTIONS: What methods are supported?
//
// 6. PATHS AND ROUTING:
//    The path (/index.html) tells the server what resource to return.
//    Real servers map paths to:
//    - Files on disk
//    - Database queries
//    - Application logic
//
// 7. MIME TYPES:
//    Content-Type uses MIME (Multipurpose Internet Mail Extensions) types:
//    - text/html, text/plain, text/css
//    - application/json, application/javascript
//    - image/jpeg, image/png, image/gif
//    - video/mp4
//    Browsers use this to know how to display/handle the data.
//
// 8. HTTP VERSIONS:
//    - HTTP/1.0: One request per connection, simple
//    - HTTP/1.1: Connection reuse, chunked encoding, more features
//    - HTTP/2: Binary protocol, multiplexing, server push
//    - HTTP/3: Uses QUIC (UDP-based) instead of TCP
//
// 9. TESTING WEB SERVERS:
//    Tools for testing HTTP servers:
//    - curl: curl http://localhost:8080
//    - wget: wget http://localhost:8080
//    - Browser: Just open the URL
//    - telnet: For manual low-level testing
//
// 10. WHY WEB SERVERS ARE MORE COMPLEX:
//    Real web servers must handle:
//    - URL parsing and routing
//    - Query parameters (?key=value)
//    - Request headers
//    - Request body (POST data)
//    - Cookies
//    - Authentication
//    - Static file serving
//    - Dynamic content generation
//    - Error handling
//    - Security (XSS, CSRF, SQL injection)
//    - Performance (caching, compression)
//
//    This example shows the absolute minimum!
//
// ============================================================================
