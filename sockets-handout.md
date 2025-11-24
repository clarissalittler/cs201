# Sockets Programming in C: From Zero to Web Server

## Introduction

By the end of this unit, you will understand how computers communicate over a network and will have built several networked applications: an echo server, a group chat system, a chat room with private messaging, and finally a web server capable of handling multiple simultaneous requests. This is one of the most practical and satisfying topics in systems programming—the code you write here is fundamentally the same as what powers the internet.

### What is a Socket?

A **socket** is an endpoint for communication between two machines. Think of it like a telephone: you need one on each end to have a conversation. Just as a phone has a number that identifies it, a socket is identified by an **IP address** (which machine) and a **port number** (which application on that machine).

When you visit a website, your browser creates a socket that connects to a socket on the web server. Data flows through this connection in both directions: your browser sends a request, and the server sends back a web page.

### The Client-Server Model

Network applications typically follow the **client-server model**:

- The **server** runs continuously, waiting for connections. It binds to a well-known port (like 80 for HTTP) so clients know where to find it.
- The **client** initiates a connection to the server when it needs something.

A single server can handle many clients. Your web browser is a client; Google's computers run servers.

### TCP vs UDP

There are two main transport protocols:

- **TCP (Transmission Control Protocol)**: Reliable, ordered delivery. If you send "Hello World", the receiver gets exactly "Hello World" or an error. TCP handles retransmission, ordering, and flow control. Web servers use TCP.

- **UDP (User Datagram Protocol)**: Unreliable, unordered. Faster but messages can be lost, duplicated, or arrive out of order. Used for video streaming, games, DNS.

We will use TCP exclusively in this unit.

---

## Part 1: The Socket API

The socket API was developed at Berkeley in the 1980s and has become the standard interface for network programming on virtually all operating systems. The core functions are defined in `<sys/socket.h>`, with additional utilities in `<netinet/in.h>`, `<arpa/inet.h>`, and `<netdb.h>`.

### Essential Data Structures

#### Socket Addresses

The `sockaddr_in` structure holds an IPv4 address and port:

```c
#include <netinet/in.h>

struct sockaddr_in {
    sa_family_t    sin_family;  // Address family (AF_INET for IPv4)
    in_port_t      sin_port;    // Port number (in network byte order)
    struct in_addr sin_addr;    // IPv4 address
};

struct in_addr {
    uint32_t s_addr;            // Address (in network byte order)
};
```

You will see functions that take `struct sockaddr *` as a parameter. This is a generic socket address type. You create a `sockaddr_in`, then cast its pointer to `sockaddr *` when calling these functions. This is a common C pattern for achieving polymorphism.

#### Network Byte Order

Different computers store multi-byte numbers differently. **Big-endian** machines store the most significant byte first; **little-endian** machines store the least significant byte first. Your x86 Linux machine is little-endian.

Networks use big-endian (called **network byte order**). You must convert values when filling in socket address structures:

```c
#include <arpa/inet.h>

uint16_t htons(uint16_t hostshort);  // Host to network short (16-bit)
uint32_t htonl(uint32_t hostlong);   // Host to network long (32-bit)
uint16_t ntohs(uint16_t netshort);   // Network to host short
uint32_t ntohl(uint32_t netlong);    // Network to host long
```

Always use these functions for port numbers and IP addresses. Forgetting to do so is a classic bug.

### The Core Functions

Here are the essential socket functions, in roughly the order you will use them:

#### socket() — Create a Socket

```c
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

Creates a new socket and returns a file descriptor (or -1 on error).

- `domain`: Address family. Use `AF_INET` for IPv4.
- `type`: Socket type. Use `SOCK_STREAM` for TCP.
- `protocol`: Usually 0 (let the system choose the appropriate protocol).

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd == -1) {
    perror("socket");
    exit(1);
}
```

The returned file descriptor works like any other in Unix—you can use `read()`, `write()`, and `close()` on it.

#### bind() — Assign an Address to a Socket

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

Associates a socket with a specific IP address and port. Servers use this to specify which port they listen on.

```c
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept connections on any interface
server_addr.sin_port = htons(8080);               // Listen on port 8080

if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("bind");
    exit(1);
}
```

`INADDR_ANY` means "accept connections on any network interface." On a machine with multiple network cards, this listens on all of them.

#### listen() — Mark Socket as Listening

```c
int listen(int sockfd, int backlog);
```

Tells the operating system that this socket will accept incoming connections. The `backlog` parameter specifies how many pending connections can queue up before the system starts rejecting them. A value of 10-128 is typical.

```c
if (listen(sockfd, 10) == -1) {
    perror("listen");
    exit(1);
}
```

#### accept() — Accept a Connection

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

Waits for a client to connect. When one does, `accept()` returns a **new** file descriptor for communicating with that specific client. The original socket continues listening for more connections.

```c
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);

int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
if (client_fd == -1) {
    perror("accept");
    exit(1);
}
// client_addr now contains the client's IP and port
```

This is a **blocking** call—it waits until a client connects.

#### connect() — Connect to a Server (Client Side)

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

Used by clients to establish a connection to a server.

```c
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("connect");
    exit(1);
}
```

#### send() and recv() — Transfer Data

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

These are like `write()` and `read()`, but specific to sockets. The `flags` parameter is usually 0.

**Critical point**: Neither function guarantees it will send/receive all the bytes you requested. Always check the return value.

```c
char *msg = "Hello, client!";
ssize_t sent = send(client_fd, msg, strlen(msg), 0);
if (sent == -1) {
    perror("send");
}

char buffer[1024];
ssize_t received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
if (received == -1) {
    perror("recv");
} else if (received == 0) {
    // Connection closed by peer
} else {
    buffer[received] = '\0';  // Null-terminate for string operations
}
```

#### close() — Close the Socket

```c
close(sockfd);
```

Closes the socket and frees resources. Always close sockets when done.

### Address Resolution with getaddrinfo()

Hardcoding IP addresses is impractical. The `getaddrinfo()` function resolves hostnames to addresses and handles both IPv4 and IPv6:

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,        // Hostname or IP
                const char *service,      // Port number or service name
                const struct addrinfo *hints,
                struct addrinfo **res);
```

Example usage:

```c
struct addrinfo hints, *res;

memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_INET;        // IPv4
hints.ai_socktype = SOCK_STREAM;  // TCP

int status = getaddrinfo("www.example.com", "80", &hints, &res);
if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(1);
}

// res now points to a linked list of results
// Use res->ai_addr and res->ai_addrlen with connect()

freeaddrinfo(res);  // Free when done
```

---

## Part 2: Building a TCP Client

Let us build a simple TCP client that connects to a server, sends a message, and prints the response. This will solidify your understanding of the client-side API.

### Complete Example: Simple TCP Client

```c
// tcp_client.c
// A simple TCP client that sends a message and prints the response.
// Compile: gcc -o tcp_client tcp_client.c
// Usage: ./tcp_client hostname port message

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s hostname port message\n", argv[0]);
        exit(1);
    }

    char *hostname = argv[1];
    char *port = argv[2];
    char *message = argv[3];

    // Step 1: Resolve the hostname
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Step 2: Create a socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(res);
        exit(1);
    }

    // Step 3: Connect to the server
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);  // No longer needed

    // Step 4: Send the message
    ssize_t sent = send(sockfd, message, strlen(message), 0);
    if (sent == -1) {
        perror("send");
        close(sockfd);
        exit(1);
    }
    printf("Sent %zd bytes: %s\n", sent, message);

    // Step 5: Receive the response
    char buffer[4096];
    ssize_t received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received == -1) {
        perror("recv");
    } else if (received == 0) {
        printf("Server closed connection.\n");
    } else {
        buffer[received] = '\0';
        printf("Received %zd bytes:\n%s\n", received, buffer);
    }

    // Step 6: Clean up
    close(sockfd);
    return 0;
}
```

You can test this against any TCP server. Later, you will test it against the servers you build.

---

## Part 3: Building a TCP Echo Server

Now let us build the server side. A TCP server follows this lifecycle:

1. Create a socket with `socket()`
2. Bind it to an address with `bind()`
3. Start listening with `listen()`
4. Accept connections in a loop with `accept()`
5. Handle each connection (read/write)
6. Close client sockets when done

### Complete Example: Echo Server

An echo server sends back whatever it receives. This is the "Hello World" of network servers.

```c
// echo_server.c
// A simple TCP echo server.
// Compile: gcc -o echo_server echo_server.c
// Usage: ./echo_server port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    // Step 1: Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    // Allow address reuse (helpful during development)
    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Step 2: Bind to address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    // Step 3: Listen for connections
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("Echo server listening on port %d...\n", port);

    // Step 4: Accept and handle connections
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;  // Keep trying
        }

        // Convert client IP to string for logging
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Step 5: Echo loop
        char buffer[1024];
        ssize_t bytes_received;

        while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            // Echo back what we received
            ssize_t bytes_sent = send(client_fd, buffer, bytes_received, 0);
            if (bytes_sent == -1) {
                perror("send");
                break;
            }
        }

        if (bytes_received == -1) {
            perror("recv");
        }

        printf("Client %s disconnected.\n", client_ip);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
```

### Important Detail: SO_REUSEADDR

Notice this line:

```c
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
```

Without this, if you stop and restart your server quickly, `bind()` will fail with "Address already in use." This happens because the operating system keeps the port reserved for a while after the socket closes (the TIME_WAIT state). `SO_REUSEADDR` allows immediate reuse. Always include this in development.

### Testing with ncat

The easiest way to test your echo server is with `ncat` (or `nc`), a versatile networking tool. Open two terminals:

**Terminal 1 (start the server):**
```bash
$ ./echo_server 8080
Echo server listening on port 8080...
```

**Terminal 2 (connect with ncat):**
```bash
$ ncat localhost 8080
Hello, server!
Hello, server!
This is a test.
This is a test.
^C
```

Everything you type gets echoed back! Press Ctrl+C to disconnect.

You can also test with your TCP client:
```bash
$ ./tcp_client localhost 8080 "Hello, server!"
Sent 14 bytes: Hello, server!
Received 14 bytes:
Hello, server!
```

### The Single-Client Limitation

Try this experiment: with the server running, open two ncat connections in separate terminals. Notice that the second connection hangs until you close the first one. Our server can only handle one client at a time because `accept()` blocks, and we do not return to it until the current client disconnects.

We will fix this limitation in the upcoming sections.

---

## Part 4: Multi-Client Echo Server

Before we build chat applications, we need a server that handles multiple clients simultaneously. Let us upgrade the echo server to use threads.

```c
// echo_server_threaded.c
// Multi-client echo server using pthreads.
// Compile: gcc -o echo_server_threaded echo_server_threaded.c -pthread
// Usage: ./echo_server_threaded port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void *handle_client(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

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

    printf("Multi-client echo server listening on port %d...\n", port);

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
        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Create a thread to handle this client
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(client_fd_ptr);
            continue;
        }
        pthread_detach(thread);
    }

    return 0;
}

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[1024];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        send(client_fd, buffer, bytes_received, 0);
    }

    printf("Client disconnected.\n");
    close(client_fd);
    return NULL;
}
```

Now test with multiple ncat connections—they all work simultaneously! Each client gets its own thread.

---

## Part 5: Group Chat Server

Now let us build something more interesting: a chat server where messages from any client are broadcast to all other connected clients. This requires:

1. Tracking all connected clients
2. Protecting the client list with a mutex (since multiple threads access it)
3. Broadcasting messages to everyone except the sender

### The Chat Server

```c
// chat_server.c
// Group chat server - messages are broadcast to all connected clients.
// Compile: gcc -o chat_server chat_server.c -pthread
// Usage: ./chat_server port

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

// Client tracking
typedef struct {
    int fd;
    int active;
    char ip[INET_ADDRSTRLEN];
    int port;
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

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

    // Initialize client array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
        clients[i].active = 0;
    }

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

        // Add to client list
        if (add_client(client_fd, client_ip, client_port) == -1) {
            char *msg = "Server full. Try again later.\n";
            send(client_fd, msg, strlen(msg), 0);
            close(client_fd);
            continue;
        }

        printf("New connection: %s:%d\n", client_ip, client_port);

        // Welcome message
        char welcome[256];
        snprintf(welcome, sizeof(welcome), 
                 "Welcome to the chat! You are %s:%d\n"
                 "Type messages and press Enter to send.\n",
                 client_ip, client_port);
        send(client_fd, welcome, strlen(welcome), 0);

        // Announce to others
        char announce[256];
        snprintf(announce, sizeof(announce), "*** %s:%d joined the chat ***\n", 
                 client_ip, client_port);
        broadcast(announce, client_fd);

        // Create handler thread
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

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    // Find this client's info for message prefixing
    char prefix[64] = "Unknown";
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == client_fd && clients[i].active) {
            snprintf(prefix, sizeof(prefix), "%s:%d", 
                     clients[i].ip, clients[i].port);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';

        // Remove trailing newline if present
        if (bytes_received > 0 && buffer[bytes_received - 1] == '\n') {
            buffer[bytes_received - 1] = '\0';
        }

        // Skip empty messages
        if (strlen(buffer) == 0) continue;

        // Format and broadcast
        char message[BUFFER_SIZE + 64];
        snprintf(message, sizeof(message), "[%s] %s\n", prefix, buffer);
        printf("%s", message);  // Log to server console
        broadcast(message, client_fd);
    }

    // Client disconnected
    printf("Client %s disconnected.\n", prefix);

    char leave_msg[256];
    snprintf(leave_msg, sizeof(leave_msg), "*** %s left the chat ***\n", prefix);
    broadcast(leave_msg, client_fd);

    remove_client(client_fd);
    close(client_fd);
    return NULL;
}

void broadcast(char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].fd != sender_fd) {
            send(clients[i].fd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

int add_client(int fd, char *ip, int port) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].fd = fd;
            clients[i].active = 1;
            strncpy(clients[i].ip, ip, INET_ADDRSTRLEN);
            clients[i].port = port;
            pthread_mutex_unlock(&clients_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return -1;  // No room
}

void remove_client(int fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == fd) {
            clients[i].active = 0;
            clients[i].fd = -1;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}
```

### Testing the Chat Server

Open three or more terminals:

**Terminal 1 (server):**
```bash
$ ./chat_server 8080
Chat server listening on port 8080...
```

**Terminals 2, 3, 4... (clients via ncat):**
```bash
$ ncat localhost 8080
Welcome to the chat! You are 127.0.0.1:54321
Type messages and press Enter to send.
```

Now messages from any client appear on all other clients!

### Understanding the Mutex

The `clients_mutex` protects the shared `clients` array. Without it, we would have a race condition: one thread might be iterating through the array to broadcast while another is modifying it (adding or removing a client). The mutex ensures only one thread accesses the array at a time.

Always follow this pattern when sharing data between threads:
```c
pthread_mutex_lock(&mutex);
// ... access shared data ...
pthread_mutex_unlock(&mutex);
```

---

## Part 6: Chat Client

Using ncat works, but let us build a proper chat client. The challenge is that we need to simultaneously wait for user input AND receive messages from the server. We will use a separate thread for receiving.

```c
// chat_client.c
// Chat client with separate threads for sending and receiving.
// Compile: gcc -o chat_client chat_client.c -pthread
// Usage: ./chat_client hostname port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int sockfd;
volatile int running = 1;

void *receive_thread(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    char *hostname = argv[1];
    char *port = argv[2];

    // Resolve hostname
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Create socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    // Connect
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    freeaddrinfo(res);
    printf("Connected to %s:%s\n", hostname, port);
    printf("Type messages and press Enter. Ctrl+C to quit.\n\n");

    // Start receive thread
    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receive_thread, NULL) != 0) {
        perror("pthread_create");
        close(sockfd);
        exit(1);
    }

    // Main thread handles sending
    char buffer[BUFFER_SIZE];
    while (running && fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            break;
        }
    }

    running = 0;
    close(sockfd);
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);

    printf("\nDisconnected.\n");
    return 0;
}

void *receive_thread(void *arg) {
    (void)arg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes;

    while (running && (bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }

    if (bytes == 0 && running) {
        printf("\nServer closed connection.\n");
        running = 0;
    }

    return NULL;
}
```

Now you can run `./chat_client localhost 8080` instead of using ncat.

---

## Part 7: Chat Room with Usernames and Private Messages

Let us build a more sophisticated chat system where users choose a username and can send private messages with `@username message`.

### The PM-Enabled Chat Server

```c
// chat_server_pm.c
// Chat server with usernames and private messaging.
// Compile: gcc -o chat_server_pm chat_server_pm.c -pthread
// Usage: ./chat_server_pm port
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

typedef struct {
    int fd;
    int active;
    char username[MAX_USERNAME];
    char ip[INET_ADDRSTRLEN];
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

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
        clients[i].username[0] = '\0';
    }

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

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    ssize_t bytes;

    // Prompt for username
    char *prompt = "Enter your username: ";
    send(client_fd, prompt, strlen(prompt), 0);

    bytes = recv(client_fd, buffer, MAX_USERNAME - 1, 0);
    if (bytes <= 0) {
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }
    buffer[bytes] = '\0';
    trim(buffer);

    // Validate username
    if (strlen(buffer) == 0) {
        char *msg = "Invalid username. Disconnecting.\n";
        send(client_fd, msg, strlen(msg), 0);
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }

    if (username_exists(buffer)) {
        char *msg = "Username already taken. Disconnecting.\n";
        send(client_fd, msg, strlen(msg), 0);
        remove_client(client_fd);
        close(client_fd);
        return NULL;
    }

    set_username(client_fd, buffer);
    char *username = get_username(client_fd);

    // Welcome message
    char welcome[512];
    snprintf(welcome, sizeof(welcome),
             "\nWelcome, %s!\n"
             "Commands:\n"
             "  @username message  - Private message\n"
             "  /who               - List users\n"
             "  /quit              - Disconnect\n\n",
             username);
    send(client_fd, welcome, strlen(welcome), 0);

    // Announce join
    char announce[256];
    snprintf(announce, sizeof(announce), "*** %s joined the chat ***\n", username);
    printf("%s", announce);
    broadcast(announce, client_fd);

    // Main message loop
    while ((bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        trim(buffer);

        if (strlen(buffer) == 0) continue;

        // Handle commands
        if (strcmp(buffer, "/quit") == 0) {
            break;
        }

        if (strcmp(buffer, "/who") == 0) {
            send_user_list(client_fd);
            continue;
        }

        // Private message: @username message
        if (buffer[0] == '@') {
            char *space = strchr(buffer, ' ');
            if (space == NULL) {
                char *err = "Usage: @username message\n";
                send(client_fd, err, strlen(err), 0);
                continue;
            }

            *space = '\0';
            char *target_user = buffer + 1;  // Skip @
            char *message = space + 1;

            if (strlen(target_user) == 0 || strlen(message) == 0) {
                char *err = "Usage: @username message\n";
                send(client_fd, err, strlen(err), 0);
                continue;
            }

            send_private(target_user, username, message, client_fd);
            continue;
        }

        // Regular broadcast message
        char message[BUFFER_SIZE + MAX_USERNAME + 8];
        snprintf(message, sizeof(message), "[%s] %s\n", username, buffer);
        printf("%s", message);
        broadcast(message, client_fd);
    }

    // Client leaving
    char *user = get_username(client_fd);
    char leave[256];
    snprintf(leave, sizeof(leave), "*** %s left the chat ***\n", user);
    printf("%s", leave);
    broadcast(leave, client_fd);

    remove_client(client_fd);
    close(client_fd);
    return NULL;
}

void broadcast(char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].fd != sender_fd) {
            send(clients[i].fd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_private(char *to_user, char *from_user, char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);

    int found = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, to_user) == 0) {
            char pm[BUFFER_SIZE + MAX_USERNAME + 16];
            snprintf(pm, sizeof(pm), "[PM from %s] %s\n", from_user, message);
            send(clients[i].fd, pm, strlen(pm), 0);

            // Confirm to sender
            char confirm[BUFFER_SIZE + MAX_USERNAME + 16];
            snprintf(confirm, sizeof(confirm), "[PM to %s] %s\n", to_user, message);
            send(sender_fd, confirm, strlen(confirm), 0);

            found = 1;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    if (!found) {
        char err[128];
        snprintf(err, sizeof(err), "User '%s' not found.\n", to_user);
        send(sender_fd, err, strlen(err), 0);
    }
}

void send_user_list(int client_fd) {
    char list[BUFFER_SIZE] = "Connected users:\n";

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strlen(clients[i].username) > 0) {
            strcat(list, "  ");
            strcat(list, clients[i].username);
            strcat(list, "\n");
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    send(client_fd, list, strlen(list), 0);
}

int add_client(int fd, char *ip) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].fd = fd;
            clients[i].active = 1;
            clients[i].username[0] = '\0';
            strncpy(clients[i].ip, ip, INET_ADDRSTRLEN);
            pthread_mutex_unlock(&clients_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return -1;
}

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

void set_username(int fd, char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == fd) {
            strncpy(clients[i].username, username, MAX_USERNAME - 1);
            clients[i].username[MAX_USERNAME - 1] = '\0';
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

char *get_username(int fd) {
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

int username_exists(char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, username) == 0) {
            pthread_mutex_unlock(&clients_mutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return 0;
}

void trim(char *str) {
    // Trim trailing whitespace
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || 
                       str[len - 1] == ' ' || str[len - 1] == '\t')) {
        str[--len] = '\0';
    }

    // Trim leading whitespace
    char *start = str;
    while (*start && (*start == ' ' || *start == '\t')) {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}
```

### Testing the PM Server

**Terminal 1:**
```bash
$ ./chat_server_pm 8080
Chat server (with PM) listening on port 8080...
```

**Terminal 2:**
```bash
$ ncat localhost 8080
Enter your username: alice

Welcome, alice!
Commands:
  @username message  - Private message
  /who               - List users
  /quit              - Disconnect

```

**Terminal 3:**
```bash
$ ncat localhost 8080
Enter your username: bob
...
```

Now alice can send a private message:
```
@bob Hey, this is private!
[PM to bob] Hey, this is private!
```

And bob sees:
```
[PM from alice] Hey, this is private!
```

The `/who` command lists all users:
```
/who
Connected users:
  alice
  bob
```

---

## Part 8: Understanding HTTP

Now that you have built several client-server applications, let us turn to the protocol that powers the web. HTTP (Hypertext Transfer Protocol) is a text-based protocol that defines how web browsers and servers communicate.

### HTTP Requests

When your browser visits `http://example.com/page.html`, it sends something like this:

```
GET /page.html HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 ...
Accept: text/html
Connection: close

```

Key elements:

- **Request line**: `METHOD PATH VERSION`
  - `GET` is the most common method (retrieve a resource)
  - `/page.html` is the path being requested
  - `HTTP/1.1` is the protocol version

- **Headers**: Key-value pairs providing metadata
  - `Host`: Required in HTTP/1.1, specifies the domain
  - `Connection: close` tells the server to close after responding

- **Blank line**: Separates headers from body (CRLF: `\r\n`)

- **Body**: Optional, used with POST requests

The request ends with a blank line (two CRLFs: `\r\n\r\n`).

### HTTP Responses

The server responds with:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 45
Connection: close

<html><body>Hello, World!</body></html>
```

Key elements:

- **Status line**: `VERSION STATUS_CODE REASON_PHRASE`
  - `200 OK` means success
  - `404 Not Found` means the resource does not exist
  - `500 Internal Server Error` means something went wrong

- **Headers**: Metadata about the response
  - `Content-Type`: MIME type of the body
  - `Content-Length`: Size of the body in bytes

- **Blank line**: Separates headers from body

- **Body**: The actual content (HTML, image, etc.)

### Common Status Codes

| Code | Meaning |
|------|---------|
| 200  | OK - Request succeeded |
| 400  | Bad Request - Malformed request |
| 403  | Forbidden - Access denied |
| 404  | Not Found - Resource does not exist |
| 500  | Internal Server Error |

### Line Endings

HTTP uses CRLF (`\r\n`) as line endings, not just LF (`\n`). This matters when parsing and generating HTTP messages.

---

## Part 9: Building a Simple Web Server

Now we combine everything to build a web server. We will start with a single-threaded version that serves static files.

### Version 1: Hardcoded Response

Let us start with the simplest possible web server—one that returns the same HTML for every request.

```c
// webserver_v1.c
// Minimal web server with hardcoded response.
// Compile: gcc -o webserver_v1 webserver_v1.c
// Usage: ./webserver_v1 port

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

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    // Read the request (we will mostly ignore it for now)
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return;
    buffer[bytes] = '\0';

    // Print the request for debugging
    printf("--- Request ---\n%s\n", buffer);

    // Prepare the response
    char *body = "<html><body><h1>Hello from C!</h1></body></html>";
    int body_len = strlen(body);

    char response[BUFFER_SIZE];
    int response_len = snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        body_len, body);

    send(client_fd, response, response_len, 0);
}
```

Test this by running the server and opening `http://localhost:8080` in your browser. You should see "Hello from C!"

### Version 2: Serving Static Files

Now let us extend the server to serve actual files from a directory. This requires parsing the HTTP request to extract the requested path.

```c
// webserver_v2.c
// Web server that serves static files.
// Compile: gcc -o webserver_v2 webserver_v2.c
// Usage: ./webserver_v2 port webroot
// Example: ./webserver_v2 8080 ./public

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUFFER_SIZE 8192
#define MAX_PATH 512

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
    webroot = argv[2];

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

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return;
    buffer[bytes] = '\0';

    // Parse the request line: GET /path HTTP/1.1
    char method[16], path[MAX_PATH], version[16];
    if (sscanf(buffer, "%15s %511s %15s", method, path, version) != 3) {
        send_error(client_fd, 400, "Bad Request");
        return;
    }

    printf("%s %s %s\n", method, path, version);

    // Only handle GET requests
    if (strcmp(method, "GET") != 0) {
        send_error(client_fd, 405, "Method Not Allowed");
        return;
    }

    // Security: reject paths with ".." to prevent directory traversal
    if (strstr(path, "..") != NULL) {
        send_error(client_fd, 403, "Forbidden");
        return;
    }

    // Build the full file path
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

    // Get file size
    struct stat st;
    fstat(fd, &st);
    int file_size = st.st_size;

    // Read file contents
    char *body = malloc(file_size);
    if (body == NULL) {
        close(fd);
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }
    read(fd, body, file_size);
    close(fd);

    // Send response
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
```

To test this:

1. Create a directory called `public` with an `index.html` file
2. Run: `./webserver_v2 8080 ./public`
3. Visit `http://localhost:8080` in your browser

---

## Part 10: Multi-Client Web Server

Our web server has the same limitation as our early echo server: it can only handle one client at a time. Let us fix that using both fork and threading approaches.

### Approach 1: Forking

When a connection arrives, we create a child process to handle it. The parent continues accepting new connections.

```c
// webserver_fork.c
// Multi-client web server using fork().
// Compile: gcc -o webserver_fork webserver_fork.c
// Usage: ./webserver_fork port webroot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>

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

    // Set up signal handler to reap zombie children
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // Restart interrupted system calls
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

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

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            close(client_fd);
            continue;
        }

        if (pid == 0) {
            // Child process
            close(server_fd);  // Child doesn't need the listening socket
            handle_client(client_fd);
            close(client_fd);
            exit(0);
        } else {
            // Parent process
            close(client_fd);  // Parent doesn't need this connection
        }
    }

    return 0;
}

void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

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
```

#### Key Points About the Fork Approach

**The SIGCHLD handler**: When a child process exits, it becomes a "zombie" until the parent calls `wait()`. The signal handler ensures we clean up these zombies automatically.

**Closing file descriptors**: After forking, each process closes the file descriptors it does not need. The child closes the listening socket; the parent closes the connected socket. This is important to avoid resource leaks.

**Process isolation**: Each request runs in its own process. If one crashes, the server keeps running. This is robust but has overhead from creating new processes.

### Approach 2: Threading

Threads are lighter-weight than processes. They share memory with the parent, which means less overhead but more care needed with shared data.

```c
// webserver_threaded.c
// Multi-client web server using pthreads.
// Compile: gcc -o webserver_threaded webserver_threaded.c -pthread
// Usage: ./webserver_threaded port webroot

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

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        int *client_fd_ptr = malloc(sizeof(int));
        if (client_fd_ptr == NULL) {
            perror("malloc");
            close(client_fd);
            continue;
        }
        *client_fd_ptr = client_fd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, client_thread, client_fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(client_fd_ptr);
            continue;
        }

        pthread_detach(thread);
    }

    return 0;
}

void *client_thread(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    handle_client(client_fd);
    close(client_fd);

    return NULL;
}

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

    printf("[Thread %lu] %s %s %s\n", (unsigned long)pthread_self(), method, path, version);

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
```

#### Key Points About the Threaded Approach

**Passing data to threads**: We allocate memory for the client file descriptor because the loop variable would change before the thread reads it. The thread frees this memory.

**pthread_detach()**: Detached threads clean themselves up when they exit. Without this, we would need to call `pthread_join()` for each thread.

**Shared memory**: All threads share global variables like `webroot`. This is convenient but dangerous—if you had shared mutable state, you would need mutexes. Our web server is stateless, so this is not an issue.

**Compile with -pthread**: The pthread library requires this flag.

---

## Part 11: Error Handling and Robustness

Production servers need to handle edge cases gracefully. Here are important considerations:

### Always Check Return Values

Every system call can fail. A robust version of send might look like:

```c
ssize_t send_all(int sockfd, char *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t sent = send(sockfd, buf + total, len - total, 0);
        if (sent == -1) {
            return -1;
        }
        total += sent;
    }
    return total;
}
```

### Handle Partial Reads

TCP is a stream protocol. A single `send()` of 1000 bytes might require multiple `recv()` calls on the other end. For HTTP, we read until we see `\r\n\r\n` marking the end of headers.

### Timeouts

A malicious client might connect and never send data, tying up server resources. Set socket timeouts:

```c
struct timeval timeout;
timeout.tv_sec = 30;
timeout.tv_usec = 0;
setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
```

### Security Considerations

- **Directory traversal**: Always check for `..` in paths
- **Buffer overflows**: Use bounded string functions (`snprintf`, not `sprintf`)
- **Resource limits**: Limit maximum connections, request size, etc.

---

## Part 12: Testing Your Applications

### Testing Servers with ncat

For any of your servers:

```bash
$ ncat localhost 8080
```

For HTTP testing, you can send raw requests:

```bash
$ echo -e "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n" | ncat localhost 8080
```

### Testing with curl

For web servers:

```bash
curl -v http://localhost:8080/
curl -v http://localhost:8080/page.html
curl -v http://localhost:8080/nonexistent.html
```

The `-v` flag shows the full HTTP request and response.

### Testing Concurrent Connections

Use a tool like `ab` (Apache Bench):

```bash
ab -n 1000 -c 10 http://localhost:8080/
```

This sends 1000 requests with 10 concurrent connections.

---

## Summary

You have now learned the fundamentals of network programming in C:

1. **Sockets** are endpoints for network communication
2. The **client-server model** structures network applications
3. **TCP** provides reliable, ordered byte streams
4. The **socket API** (`socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`) enables network programming
5. **Threading** allows handling multiple clients simultaneously
6. **Mutexes** protect shared data from race conditions
7. **HTTP** is a text-based request-response protocol
8. **Multi-client servers** can use forking, threading, or I/O multiplexing

The applications you built—echo server, group chat, private messaging chat, and web server—demonstrate progressively more complex uses of these concepts. Real production servers like nginx and Apache are more sophisticated—they use non-blocking I/O, connection pooling, caching, and many optimizations—but they build on exactly these foundations.

---

## Exercises

1. **Add timestamps**: Modify the chat server to prefix each message with a timestamp.

2. **Chat rooms**: Extend the PM chat server to support multiple rooms. Users can `/join #roomname` and messages only go to users in the same room.

3. **File transfer**: Create a simple file transfer protocol where a client can upload and download files from the server.

4. **Support POST**: Extend the web server to handle POST requests. Parse the request body and echo it back as HTML.

5. **Directory listing**: If a web request is for a directory without an index.html, generate and return an HTML page listing the directory contents.

6. **Persistent connections**: HTTP/1.1 keeps connections open by default. Modify the web server to handle multiple requests per connection.

7. **Thread pool**: Instead of creating a new thread per request, create a fixed pool of worker threads that process requests from a queue.

8. **Benchmark**: Compare the performance of the single-threaded, forking, and threaded web servers under load.

---

## Further Reading

- Beej's Guide to Network Programming: https://beej.us/guide/bgnet/
- Stevens, "UNIX Network Programming" (the definitive reference)
- RFC 2616 (HTTP/1.1 specification)
- `man 2 socket`, `man 7 tcp`, `man 7 ip` (Linux manual pages)
