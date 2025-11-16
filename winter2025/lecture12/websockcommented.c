// PEDAGOGICAL PURPOSE:
// This program demonstrates a MINIMAL HTTP WEB SERVER that responds to browser requests.
// Key learning objectives:
// 1. Understanding the HTTP protocol basics (request-response pattern)
// 2. Learning that HTTP runs on top of TCP sockets (just like our echo server)
// 3. Understanding HTTP response format (status line, headers, body)
// 4. Learning how web browsers communicate with servers
// 5. Seeing how to serve HTML content over sockets
// 6. Understanding the difference between low-level sockets and application protocols
// 7. Learning about Content-Type and Content-Length HTTP headers
// 8. Understanding that a web server is just a specialized echo server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// DEFINED CONSTANTS:
// Using #define to create named constants
// WHY #define INSTEAD OF int?
//   - Preprocessor constants - replaced before compilation
//   - No memory used at runtime
//   - Convention for constants in C
//   - Makes code more maintainable
//
// #define PORT 8080
//   - The port number for our web server
//   - Web browsers typically use port 80 (HTTP) or 443 (HTTPS)
//   - 8080 is common for development web servers
//   - Doesn't require root privileges (ports < 1024 do)
//
// #define BUFFER_SIZE 1024
//   - Size of buffer for receiving HTTP requests
//   - 1024 bytes = 1 KB
//   - Typical HTTP request headers fit in this size
//   - Real servers use larger buffers or dynamic allocation
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // SOCKET FILE DESCRIPTORS:
    // Same as previous socket programs
    int server_fd, new_socket;

    // ADDRESS STRUCTURE:
    // Same as previous programs - holds IP and port info
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // REQUEST BUFFER:
    // Initialized to all zeros using {0}
    // This is a C99 feature for zero-initialization
    // Equivalent to: memset(buffer, 0, BUFFER_SIZE)
    // WHY ZERO-INITIALIZE?
    //   - Ensures buffer is clean from the start
    //   - Prevents garbage data if we don't read full buffer
    //   - Makes debugging easier (empty = all zeros)
    char buffer[BUFFER_SIZE] = {0};

    // STEP 1: CREATE SOCKET
    // Same as all previous examples
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Creating socket file descriptor
    // AF_INET: IPv4
    // SOCK_STREAM: TCP (reliable, connection-oriented)
    // 0: Default protocol (TCP)

    // STEP 2: CONFIGURE ADDRESS STRUCTURE
    // Same as previous examples
    address.sin_family = AF_INET;  // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address
    address.sin_port = htons(PORT);  // Set port number, converting to network byte order

    // STEP 3: BIND SOCKET
    // Same as previous examples
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Bind the socket to the address and port
    // This claims port 8080 for our web server

    // STEP 4: LISTEN FOR CONNECTIONS
    // Same as previous examples
    listen(server_fd, 3);

    // Listen for incoming connections
    // Backlog of 3 - maximum pending connections in queue

    // INFORMATIVE OUTPUT:
    // Let user know the server is ready
    printf("Web server listening on port %d\n", PORT);

    // MAIN SERVER LOOP:
    // This is an INFINITE loop - server runs forever
    // Different from sock1.c (one client) and sock3.c (5 clients)
    //
    // WHY while(1)?
    //   - Web servers need to run continuously
    //   - Handle requests from browsers indefinitely
    //   - Only stop when killed (Ctrl+C)
    //   - Common pattern for production servers
    while (1) {

        // CLEAR BUFFER:
        // Reset buffer to all zeros before each request
        // WHY NECESSARY?
        //   - Previous request data might still be in buffer
        //   - HTTP requests vary in size
        //   - Ensures we don't see old data mixed with new
        //
        // ALTERNATIVE:
        //   - Could declare buffer inside loop (auto-cleared on each iteration)
        //   - But memset is more explicit and clear
        memset(buffer, 0, BUFFER_SIZE);

        // STEP 5: ACCEPT A CLIENT (BROWSER) CONNECTION
        // Each iteration handles ONE HTTP request
        //
        // IMPORTANT: HTTP is typically stateless
        //   - Browser connects
        //   - Browser sends request
        //   - Server sends response
        //   - Connection closes
        //   - Next request = new connection
        //
        // This is different from our echo servers where one connection
        // handled multiple messages
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        // STEP 6: READ HTTP REQUEST FROM BROWSER
        // Browser sends an HTTP request (text-based protocol)
        //
        // EXAMPLE HTTP REQUEST:
        //   GET / HTTP/1.1
        //   Host: localhost:8080
        //   User-Agent: Mozilla/5.0 (X11; Linux x86_64)...
        //   Accept: text/html,application/xhtml+xml...
        //   Accept-Language: en-US,en;q=0.5
        //   Accept-Encoding: gzip, deflate
        //   Connection: keep-alive
        //   (blank line)
        //
        // KEY PARTS:
        //   - Request line: "GET / HTTP/1.1"
        //     - Method: GET (fetch a resource)
        //     - Path: / (root/homepage)
        //     - Version: HTTP/1.1
        //   - Headers: Key-value pairs with metadata
        //   - Blank line: Signals end of headers
        //   - Body: (not present in GET requests)
        read(new_socket, buffer, BUFFER_SIZE);  // Read incoming request from client

        // DISPLAY THE HTTP REQUEST:
        // Print the entire request to server console
        // Useful for debugging - see exactly what browser sent
        // This is educational - production servers would parse, not just print
        printf("Received request:\n%s\n", buffer);  // Print the HTTP request

        // STEP 7: PREPARE HTTP RESPONSE
        // HTTP response is also text-based
        //
        // RESPONSE STRUCTURE:
        //   Status Line
        //   Headers (one per line)
        //   Blank line
        //   Body (the actual content)
        //
        // OUR RESPONSE BREAKDOWN:
        //
        // "HTTP/1.1 200 OK\r\n"
        //   - Status Line
        //   - HTTP/1.1: Protocol version
        //   - 200: Status code (OK - success)
        //   - OK: Status message (human-readable)
        //   - \r\n: Carriage return + line feed (HTTP line ending)
        //
        // "Content-Type: text/html\r\n"
        //   - Header line
        //   - Content-Type: Tells browser what kind of data we're sending
        //   - text/html: We're sending HTML (web page)
        //   - Could be: text/plain, application/json, image/png, etc.
        //   - \r\n: End of this header line
        //
        // "Content-Length: 46\r\n"
        //   - Header line
        //   - Content-Length: How many bytes is the body?
        //   - 46: The HTML string has exactly 46 bytes
        //   - Browser uses this to know when response is complete
        //   - Count: "<html><body><h1>Hello, World!</h1></body></html>"
        //            = 46 characters
        //   - \r\n: End of this header line
        //
        // "\r\n"
        //   - Blank line
        //   - CRITICAL: Separates headers from body
        //   - Headers end, body begins
        //
        // "<html><body><h1>Hello, World!</h1></body></html>"
        //   - The body (actual content)
        //   - Valid HTML document
        //   - Browser will parse and display this
        //   - Creates a heading "Hello, World!"
        //
        // WHY \r\n (not just \n)?
        //   - HTTP spec requires CRLF (Carriage Return + Line Feed)
        //   - \r = Carriage Return (CR) = ASCII 13
        //   - \n = Line Feed (LF) = ASCII 10
        //   - This is from teletype days
        //   - \n alone works with some browsers, but \r\n is correct
        char *response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n"
                         "Content-Length: 46\r\n"
                         "\r\n"
                         "<html><body><h1>Hello, World!</h1></body></html>";

        // STEP 8: SEND HTTP RESPONSE TO BROWSER
        // Same as previous socket examples - write to socket
        //
        // IMPORTANT: We send the ENTIRE response (status + headers + body)
        // Browser will parse this text and:
        //   1. Check status code (200 = OK, show content)
        //   2. Read headers (especially Content-Type)
        //   3. Read body (until Content-Length bytes received)
        //   4. Render the HTML
        write(new_socket, response, strlen(response));  // Send response to client

        // STEP 9: CLOSE THIS CONNECTION
        // HTTP/1.0 behavior: One request, one response, close
        //
        // WHY CLOSE IMMEDIATELY?
        //   - Simpler server implementation
        //   - Each request is independent
        //   - HTTP/1.1 can use persistent connections (Connection: keep-alive)
        //   - But our simple server closes after each response
        //
        // WHAT HAPPENS NEXT?
        //   - Browser receives response
        //   - Connection closes
        //   - Browser displays "Hello, World!"
        //   - Server loops back to accept() for next request
        close(new_socket);  // Close client connection
    }
    // Loop continues - server never exits (unless killed)

    // UNREACHABLE CODE:
    // This line never executes because while(1) is infinite
    // Good practice to include it anyway for code clarity
    close(server_fd);  // Close server socket
    return 0;
}

// EXPECTED OUTPUT (SERVER CONSOLE):
// Web server listening on port 8080
// Received request:
// GET / HTTP/1.1
// Host: localhost:8080
// User-Agent: Mozilla/5.0...
// Accept: text/html...
// (blank line)
//
// Received request:
// GET /favicon.ico HTTP/1.1
// Host: localhost:8080
// ...
// (browsers often request favicon too)

// EXPECTED OUTPUT (BROWSER):
// When you navigate to http://localhost:8080
// You see a web page with the heading: "Hello, World!"

// TESTING THE WEB SERVER:
//
// Terminal 1 (Server):
//   $ gcc websock.c -o websock
//   $ ./websock
//   Web server listening on port 8080
//
// Browser:
//   Open: http://localhost:8080
//   See: "Hello, World!" as a heading
//
// OR use curl:
//   $ curl http://localhost:8080
//   <html><body><h1>Hello, World!</h1></body></html>
//
// OR use telnet:
//   $ telnet localhost 8080
//   GET / HTTP/1.1
//   Host: localhost
//   (press Enter twice)
//   (see HTTP response)

// HTTP STATUS CODES:
//
// Common codes you should know:
//   200 OK - Request succeeded
//   201 Created - Resource created successfully
//   204 No Content - Success but no content to return
//   301 Moved Permanently - Resource has moved to new URL
//   302 Found (Temporary Redirect)
//   304 Not Modified - Cached version is still valid
//   400 Bad Request - Malformed request
//   401 Unauthorized - Authentication required
//   403 Forbidden - Server understood but refuses
//   404 Not Found - Resource doesn't exist
//   500 Internal Server Error - Server crashed/error
//   502 Bad Gateway - Invalid response from upstream server
//   503 Service Unavailable - Server temporarily down

// HTTP METHODS (VERBS):
//
// Common methods:
//   GET - Retrieve a resource (like our example)
//   POST - Submit data to server (forms, uploads)
//   PUT - Replace/update a resource
//   DELETE - Remove a resource
//   HEAD - Like GET but only retrieve headers
//   OPTIONS - Ask what methods are supported
//   PATCH - Partial update of resource

// COMMON MISTAKES:
//
// MISTAKE 1: Wrong Content-Length
//   "Content-Length: 50\r\n"  // But HTML is 46 bytes
//   Result: Browser waits for 4 more bytes, hangs
//   Fix: Count exact bytes in body
//
// MISTAKE 2: Missing blank line between headers and body
//   "Content-Length: 46\r\n"
//   "<html>..."  // Missing \r\n before body
//   Result: Body is interpreted as headers, broken response
//   Fix: Always include "\r\n" after last header
//
// MISTAKE 3: Using \n instead of \r\n
//   "HTTP/1.1 200 OK\n"
//   Result: Works with some browsers, but not HTTP compliant
//   Fix: Always use \r\n for HTTP line endings
//
// MISTAKE 4: Not closing connection
//   // Missing close(new_socket)
//   Result: Browser hangs waiting for more data
//   Fix: Always close after sending response
//
// MISTAKE 5: Wrong Content-Type
//   "Content-Type: text/plain\r\n"
//   Result: Browser shows HTML source code instead of rendering
//   Fix: Use text/html for HTML content

// LIMITATIONS OF THIS WEB SERVER:
//
// 1. SERVES ONLY ONE PAGE:
//    - Always returns "Hello, World!" regardless of requested path
//    - Requesting /about, /contact, /image.jpg all return same thing
//    - Real server would parse request path and serve different content
//
// 2. NO FILE SERVING:
//    - Response is hardcoded in memory
//    - Can't serve actual HTML files from disk
//    - Real server would read files and send their contents
//
// 3. IGNORES REQUEST CONTENT:
//    - Reads request but doesn't parse it
//    - Doesn't check method (GET, POST, etc.)
//    - Doesn't check path (/, /about, etc.)
//    - Just prints it and sends same response
//
// 4. NO ERROR HANDLING:
//    - What if requested file doesn't exist? (should return 404)
//    - What if method is not supported? (should return 405)
//    - All requests get 200 OK
//
// 5. ONE CONNECTION AT A TIME:
//    - Sequential handling (like sock3.c)
//    - Second browser must wait if first is active
//    - Real servers use threads/processes/async I/O
//
// 6. NO SECURITY:
//    - No HTTPS (encryption)
//    - No authentication
//    - No input validation
//    - Vulnerable to attacks

// HOW TO IMPROVE THIS SERVER:
//
// IMPROVEMENT 1: Parse request path
//   char method[16], path[256], version[16];
//   sscanf(buffer, "%s %s %s", method, path, version);
//   if (strcmp(path, "/") == 0) { serve_homepage(); }
//   else if (strcmp(path, "/about") == 0) { serve_about(); }
//   else { send_404(); }
//
// IMPROVEMENT 2: Serve files from disk
//   FILE* f = fopen("index.html", "r");
//   fread(file_buffer, 1, file_size, f);
//   sprintf(response, "HTTP/1.1 200 OK\r\n...Content-Length: %d\r\n\r\n%s",
//           file_size, file_buffer);
//
// IMPROVEMENT 3: Handle different Content-Types
//   if (ends_with(path, ".html")) type = "text/html";
//   else if (ends_with(path, ".jpg")) type = "image/jpeg";
//   else if (ends_with(path, ".css")) type = "text/css";
//
// IMPROVEMENT 4: Implement 404 for missing pages
//   if (file_not_found(path)) {
//     response = "HTTP/1.1 404 Not Found\r\n"
//                "Content-Type: text/html\r\n"
//                "Content-Length: 23\r\n\r\n"
//                "<h1>404 Not Found</h1>";
//   }

// REAL-WORLD WEB SERVERS:
//
// Apache HTTP Server:
//   - Hundreds of thousands of lines of code
//   - Handles thousands of concurrent connections
//   - Supports modules, virtual hosts, SSL/TLS
//   - Configuration files, access control, logging
//
// nginx:
//   - Event-driven architecture
//   - High performance for static content
//   - Reverse proxy capabilities
//   - Load balancing
//
// Our server vs. Apache:
//   - We: ~50 lines
//   - Apache: ~500,000 lines
//   - But the CORE concept is the same: accept, read, respond, close

// HTTP/1.1 vs HTTP/2 vs HTTP/3:
//
// HTTP/1.1 (our implementation):
//   - Text-based protocol
//   - One request per connection (or sequential with keep-alive)
//   - Human-readable
//
// HTTP/2:
//   - Binary protocol
//   - Multiplexing (multiple requests on one connection)
//   - Header compression
//   - Server push
//
// HTTP/3:
//   - Based on QUIC (UDP instead of TCP)
//   - Faster connection establishment
//   - Better handling of packet loss
//   - Still uses HTTP/2 concepts

// THE REQUEST-RESPONSE CYCLE:
//
// 1. Browser needs resource (user types URL or clicks link)
// 2. Browser establishes TCP connection to server (3-way handshake)
// 3. Browser sends HTTP request (text)
// 4. Server receives request
// 5. Server processes request (our server just sends canned response)
// 6. Server sends HTTP response (status + headers + body)
// 7. Browser receives response
// 8. Connection closes (in our simple server)
// 9. Browser renders HTML (if Content-Type is text/html)

// WHY THIS MATTERS:
//
// Understanding that HTTP runs on TCP sockets is CRUCIAL:
//   - Web frameworks (Flask, Express, Spring) ultimately use sockets
//   - When you see "listen on port 8080" in a web framework, this is it
//   - APIs, web services, REST all use HTTP on TCP sockets
//   - The abstractions hide the sockets, but they're still there
//   - This program reveals what's happening under the hood

// TRY IT:
// 1. Compile: gcc websock.c -o websock
// 2. Run: ./websock
// 3. Open browser: http://localhost:8080
// 4. See "Hello, World!" heading
// 5. Check server console - see the HTTP request
// 6. EXPERIMENT: Change "Hello, World!" to "Welcome to my server!"
//    Recompile and run, refresh browser
// 7. EXPERIMENT: Change Content-Type to "text/plain"
//    See how browser displays raw HTML instead of rendering
// 8. EXPERIMENT: Change status code to "404 Not Found"
//    See browser's default 404 page
// 9. EXPERIMENT: Add more HTML to the body (add <p> tags, <img>, etc.)
//    Update Content-Length to match new size
// 10. EXPERIMENT: Use curl -v http://localhost:8080
//     See full HTTP conversation (request and response)
// 11. CHALLENGE: Parse the request path and serve different content for different paths
//     Hint: Use sscanf to extract path from first line of request
// 12. CHALLENGE: Read an actual HTML file from disk and serve it
// 13. CHALLENGE: Implement 404 response for path != "/"
// 14. CHALLENGE: Count visits and display number in the HTML response
