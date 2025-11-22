# Lecture 12 - Network Programming with Sockets

This directory contains examples for Winter 2025 Lecture 12, demonstrating network programming using TCP/IP sockets, client-server architecture, and web protocols.

## Contents

### TCP Socket Examples
- **sock1.c** - Basic TCP server
- **sock1commented.c** - Detailed explanation of server socket programming
- **sock2.c** - TCP client
- **sock2commented.c** - Complete client socket tutorial
- **sock3.c** - Advanced server with multiple client handling
- **sock3commented.c** - Multi-client server implementation

### Web Programming
- **websock.c** - Simple HTTP web server
- **websockcommented.c** - HTTP protocol and web server implementation

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make sock1
make sock2
make websock
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic Client-Server

**Terminal 1 (Server):**
```bash
./sock1
```
Server listens on port 8080 (or specified port).

**Terminal 2 (Client):**
```bash
./sock2
```
Client connects to server and exchanges messages.

**Expected behavior:**
- Server accepts connection
- Client sends message
- Server receives and responds
- Bidirectional communication

### Multi-Client Server

**Terminal 1 (Server):**
```bash
./sock3
```

**Terminal 2-N (Multiple Clients):**
```bash
./sock2
```

**Expected behavior:**
- Server handles multiple clients concurrently
- Each client has independent connection
- Uses threads or select() for concurrent handling

### Web Server

```bash
./websock
```

**Then in web browser:**
```
http://localhost:8080/
```

**Expected behavior:**
- Server responds to HTTP requests
- Serves simple web pages
- Demonstrates HTTP protocol

## Key Concepts Demonstrated

### TCP/IP Sockets

**What is a socket?**
- Communication endpoint
- Identified by IP address + port number
- Abstraction over network protocols

### Socket Types
- **SOCK_STREAM**: TCP (reliable, connection-oriented)
- **SOCK_DGRAM**: UDP (unreliable, connectionless)
- **SOCK_RAW**: Raw network access

### Server Socket Programming

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 1. Create socket
int server_fd = socket(AF_INET, SOCK_STREAM, 0);

// 2. Set up address structure
struct sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;  // Any interface
address.sin_port = htons(8080);        // Port 8080

// 3. Bind socket to address
bind(server_fd, (struct sockaddr*)&address, sizeof(address));

// 4. Listen for connections
listen(server_fd, 5);  // Backlog of 5

// 5. Accept incoming connection
int client_fd = accept(server_fd, NULL, NULL);

// 6. Read/write data
char buffer[1024];
read(client_fd, buffer, sizeof(buffer));
write(client_fd, "Response", 8);

// 7. Close sockets
close(client_fd);
close(server_fd);
```

### Client Socket Programming

```c
// 1. Create socket
int sock = socket(AF_INET, SOCK_STREAM, 0);

// 2. Set up server address
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

// 3. Connect to server
connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

// 4. Send/receive data
write(sock, "Hello", 5);
char buffer[1024];
read(sock, buffer, sizeof(buffer));

// 5. Close socket
close(sock);
```

### Important Socket Functions

#### socket()
```c
int socket(int domain, int type, int protocol);
```
- **domain**: AF_INET (IPv4), AF_INET6 (IPv6)
- **type**: SOCK_STREAM (TCP), SOCK_DGRAM (UDP)
- **protocol**: Usually 0 (auto-select)

#### bind()
```c
int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
```
- Associates socket with address and port
- Server-side operation
- Returns -1 on error

#### listen()
```c
int listen(int sockfd, int backlog);
```
- Marks socket as passive (ready to accept)
- **backlog**: Max pending connections

#### accept()
```c
int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
```
- Blocks until client connects
- Returns new socket for communication
- Original socket continues listening

#### connect()
```c
int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
```
- Client-side operation
- Establishes connection to server
- Blocks until connected or timeout

### Port Numbers

**Well-known ports (0-1023):**
- 20/21: FTP
- 22: SSH
- 23: Telnet
- 25: SMTP
- 53: DNS
- 80: HTTP
- 443: HTTPS

**Registered ports (1024-49151):**
- Application-specific

**Dynamic/Private (49152-65535):**
- Temporary client ports

**For development:**
- Use ports > 1024 to avoid needing root

### Network Byte Order

**Problem:** Different CPUs use different byte ordering

**Solution:** Network byte order (big-endian)

```c
// Host to network (short)
uint16_t port = htons(8080);

// Host to network (long)
uint32_t addr = htonl(INADDR_ANY);

// Network to host (short)
uint16_t local_port = ntohs(port);

// Network to host (long)
uint32_t local_addr = ntohl(addr);
```

### IP Address Conversion

```c
// String to binary
struct in_addr addr;
inet_pton(AF_INET, "192.168.1.1", &addr);

// Binary to string
char ip_str[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &addr, ip_str, sizeof(ip_str));
```

### Socket Options

```c
// Allow address reuse (useful for development)
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// Set receive timeout
struct timeval tv;
tv.tv_sec = 5;
tv.tv_usec = 0;
setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
```

### Handling Multiple Clients

**Option 1: Threads**
```c
while (1) {
    int client = accept(server_fd, NULL, NULL);
    pthread_t thread;
    pthread_create(&thread, NULL, handle_client, &client);
    pthread_detach(thread);
}
```

**Option 2: select()**
```c
fd_set readfds;
while (1) {
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    select(server_fd + 1, &readfds, NULL, NULL, NULL);
    if (FD_ISSET(server_fd, &readfds)) {
        int client = accept(server_fd, NULL, NULL);
        // Handle client...
    }
}
```

**Option 3: fork()**
```c
while (1) {
    int client = accept(server_fd, NULL, NULL);
    if (fork() == 0) {
        // Child process handles client
        close(server_fd);
        handle_client(client);
        exit(0);
    }
    close(client);  // Parent closes client socket
}
```

### HTTP Basics

**HTTP Request:**
```
GET /index.html HTTP/1.1
Host: localhost
User-Agent: Mozilla/5.0
```

**HTTP Response:**
```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 123

<html>...</html>
```

**Simple HTTP server:**
```c
// Read request
char request[1024];
read(client, request, sizeof(request));

// Send response
char* response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!";
write(client, response, strlen(response));
```

## Troubleshooting

### Address already in use?
```c
int opt = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

### Connection refused?
- Check server is running
- Verify port number matches
- Check firewall settings

### Broken pipe?
- Client disconnected
- Check return value of write()
- Handle SIGPIPE signal

### Can't bind to port < 1024?
- Requires root privileges
- Use port > 1024 for development

## Security Considerations

**Important:**
- Never trust client input
- Validate all data
- Prevent buffer overflows
- Use timeouts
- Limit resource usage
- Consider encryption (TLS/SSL)

## Best Practices

1. **Always check return values** from socket functions
2. **Use SO_REUSEADDR** during development
3. **Set timeouts** to prevent hanging
4. **Handle signals** (SIGPIPE especially)
5. **Close sockets** when done
6. **Validate input** from network
7. **Use threads or async I/O** for scalability

## Common Patterns

### Echo Server
- Reads data from client
- Sends same data back
- Good for testing

### Chat Server
- Broadcasts messages to all clients
- Maintains list of connections

### HTTP Server
- Parses HTTP requests
- Serves files or generates responses
- Handles different URLs

## Notes

- Sockets are the foundation of network programming
- TCP provides reliable, ordered delivery
- Understanding socket API is essential for network applications
- Error handling is critical in network programming
- Modern alternatives include higher-level libraries (libcurl, etc.)
- Network programming introduces concurrency challenges
- Always test with multiple clients
- Use tools like netcat, telnet, curl for testing
- Monitor with netstat, ss, lsof
