// tcp_client_commented.c
// A simple TCP client that sends a message and prints the response.
// This example demonstrates the fundamental steps of creating a TCP client using sockets.
//
// Compile: gcc -o tcp_client_commented tcp_client_commented.c
// Usage: ./tcp_client_commented hostname port message

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      // For close()
#include <sys/types.h>   // For socket types
#include <sys/socket.h>  // For socket(), connect(), send(), recv()
#include <netdb.h>       // For getaddrinfo() and related structures

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s hostname port message\n", argv[0]);
        exit(1);
    }

    char *hostname = argv[1];
    char *port = argv[2];
    char *message = argv[3];

    // ========================================================================
    // STEP 1: RESOLVE THE HOSTNAME
    // ========================================================================
    // Before we can connect to a server, we need to convert the human-readable
    // hostname and port into a network address structure that the OS can use.
    // This is called "name resolution" or "address resolution".

    struct addrinfo hints, *res;

    // The 'hints' structure tells getaddrinfo() what kind of address we want.
    // Always zero it out first to ensure all fields start with known values.
    memset(&hints, 0, sizeof(hints));

    // AF_INET = IPv4 addresses only
    // (could also use AF_INET6 for IPv6, or AF_UNSPEC to accept either)
    hints.ai_family = AF_INET;

    // SOCK_STREAM = TCP (connection-oriented, reliable, ordered byte stream)
    // (vs SOCK_DGRAM for UDP, which is connectionless and unreliable)
    hints.ai_socktype = SOCK_STREAM;

    // getaddrinfo() does the DNS lookup and fills in a linked list of possible addresses.
    // It's the modern, protocol-independent way to resolve hostnames.
    // Parameters:
    //   hostname - the server name or IP address (e.g., "example.com" or "127.0.0.1")
    //   port     - the port number as a string (e.g., "80" or "http")
    //   &hints   - our preferences for what kind of address we want
    //   &res     - OUTPUT: pointer to a linked list of address structures
    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0) {
        // gai_strerror() converts getaddrinfo error codes to human-readable strings
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // At this point, 'res' points to a linked list of addrinfo structures.
    // Each structure contains a socket address we could try to connect to.
    // We'll just use the first one (res), but production code might iterate
    // through them and try each until one succeeds.

    // ========================================================================
    // STEP 2: CREATE A SOCKET
    // ========================================================================
    // A socket is an endpoint for network communication. Think of it like
    // opening a file - you get a file descriptor (an integer) that represents
    // the connection.

    // socket() creates a new socket and returns a file descriptor.
    // We pass in the address family, socket type, and protocol from the
    // addrinfo structure that getaddrinfo() filled in for us.
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(res);
        exit(1);
    }

    // WHY use res->ai_family/ai_socktype/ai_protocol instead of hardcoding?
    // Because getaddrinfo() handles the details of IPv4 vs IPv6 for us.
    // This makes our code more portable and future-proof.

    // ========================================================================
    // STEP 3: CONNECT TO THE SERVER
    // ========================================================================
    // For TCP clients, connect() establishes a connection to the server.
    // This is where the TCP three-way handshake (SYN, SYN-ACK, ACK) happens.

    // connect() parameters:
    //   sockfd         - the socket we created
    //   res->ai_addr   - pointer to the server's address structure
    //   res->ai_addrlen - size of the address structure
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }

    // After successful connect(), the socket is now connected and ready
    // to send/receive data. The TCP connection is established.

    // We're done with the addrinfo structure, so free it.
    // Always call freeaddrinfo() after you're done to prevent memory leaks.
    freeaddrinfo(res);

    // ========================================================================
    // STEP 4: SEND DATA
    // ========================================================================
    // send() transmits data over a connected socket.

    // send() parameters:
    //   sockfd          - the connected socket
    //   message         - pointer to the data to send
    //   strlen(message) - number of bytes to send
    //   0               - flags (0 = no special options)
    //
    // IMPORTANT: send() returns the number of bytes actually sent.
    // It might not send all the data in one call! Production code should
    // loop to ensure all data is sent, but this example is simplified.
    ssize_t sent = send(sockfd, message, strlen(message), 0);
    if (sent == -1) {
        perror("send");
        close(sockfd);
        exit(1);
    }
    printf("Sent %zd bytes: %s\n", sent, message);

    // NOTE: send() doesn't send a null terminator automatically!
    // We're only sending strlen(message) bytes, not including '\0'.
    // If the server needs to know where the message ends, you need to
    // either include a delimiter, send the length first, or use a protocol.

    // ========================================================================
    // STEP 5: RECEIVE THE RESPONSE
    // ========================================================================
    // recv() receives data from a connected socket.

    char buffer[4096];

    // recv() parameters:
    //   sockfd              - the connected socket
    //   buffer              - buffer to store received data
    //   sizeof(buffer) - 1  - maximum bytes to receive (leave room for '\0')
    //   0                   - flags (0 = no special options)
    //
    // recv() returns:
    //   > 0  - number of bytes received
    //   0    - connection closed by peer (EOF)
    //   -1   - error occurred
    ssize_t received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received == -1) {
        perror("recv");
    } else if (received == 0) {
        // recv() returning 0 means the server closed the connection gracefully.
        // This is the TCP equivalent of EOF when reading a file.
        printf("Server closed connection.\n");
    } else {
        // We received some data! Null-terminate it so we can print it as a string.
        // This is safe because we left room for the '\0' by using sizeof(buffer) - 1.
        buffer[received] = '\0';
        printf("Received %zd bytes:\n%s\n", received, buffer);
    }

    // IMPORTANT: Like send(), recv() might not receive all available data in
    // one call. It returns when it has *some* data, not necessarily all of it.
    // If you need to receive a specific amount of data, you need to loop.

    // ========================================================================
    // STEP 6: CLEAN UP
    // ========================================================================
    // Always close sockets when done to free system resources.
    // close() also sends a FIN packet to gracefully close the TCP connection.
    close(sockfd);

    return 0;
}

// ============================================================================
// KEY SOCKET CONCEPTS DEMONSTRATED:
// ============================================================================
//
// 1. SOCKET FILE DESCRIPTORS:
//    Sockets are represented as file descriptors (integers), just like files.
//    You can use them with select(), poll(), epoll(), etc.
//
// 2. BLOCKING I/O:
//    By default, connect(), send(), and recv() are BLOCKING operations.
//    They will wait until they complete (or error). This is fine for simple
//    programs but limits scalability. See select/poll/epoll for non-blocking.
//
// 3. TCP GUARANTEES:
//    - Reliable: Data is guaranteed to arrive (or you get an error)
//    - Ordered: Bytes arrive in the same order they were sent
//    - Stream-oriented: No message boundaries (unlike UDP)
//
// 4. ERROR HANDLING:
//    Always check return values! Network operations can fail in many ways:
//    - DNS lookup fails (hostname doesn't exist)
//    - Connection refused (no server listening on that port)
//    - Connection timeout (firewall, server down, network issue)
//    - Connection reset (server crashed or forcibly closed)
//
// 5. RESOURCE MANAGEMENT:
//    - Always free allocated memory (freeaddrinfo)
//    - Always close file descriptors (close)
//    - These prevent resource leaks
//
// ============================================================================
