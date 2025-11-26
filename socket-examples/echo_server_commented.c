// echo_server_commented.c
// A simple TCP echo server that handles one client at a time.
// Demonstrates the fundamental steps for creating a TCP server.
//
// Compile: gcc -o echo_server_commented echo_server_commented.c
// Usage: ./echo_server_commented port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>       // For close()
#include <sys/types.h>    // For socket types
#include <sys/socket.h>   // For socket(), bind(), listen(), accept()
#include <netinet/in.h>   // For sockaddr_in structure and INADDR_ANY
#include <arpa/inet.h>    // For inet_ntop() to convert IP addresses to strings

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    // ========================================================================
    // STEP 1: CREATE A SOCKET
    // ========================================================================
    // Just like the client, we start by creating a socket.
    // However, this socket will be used for LISTENING for connections,
    // not for connecting to a remote server.

    // socket() parameters:
    //   AF_INET      - IPv4 addresses
    //   SOCK_STREAM  - TCP (reliable, connection-oriented)
    //   0            - Let the system choose the appropriate protocol (TCP)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    // ========================================================================
    // SOCKET OPTION: SO_REUSEADDR
    // ========================================================================
    // This is a common idiom that's important for server development.
    // Without this, if you stop and restart your server quickly, you might get
    // "Address already in use" error. Here's why:
    //
    // When a TCP connection closes, it enters a TIME_WAIT state (typically 60 seconds)
    // to ensure all packets have been delivered. During this time, the OS won't
    // let you bind to that port again by default.
    //
    // SO_REUSEADDR tells the OS: "Let me bind to this port even if it's in TIME_WAIT."
    // This is safe for servers and makes development much easier!

    int optval = 1;  // 1 = enable the option

    // setsockopt() parameters:
    //   server_fd       - the socket to configure
    //   SOL_SOCKET      - we're setting a socket-level option (not protocol-specific)
    //   SO_REUSEADDR    - the specific option to set
    //   &optval         - pointer to the value (1 = enable)
    //   sizeof(optval)  - size of the value
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // NOTE: We're not checking the return value here. In production code you might,
    // but SO_REUSEADDR is an optimization - the server works without it, just less conveniently.

    // ========================================================================
    // STEP 2: BIND TO AN ADDRESS AND PORT
    // ========================================================================
    // Binding associates the socket with a specific IP address and port number.
    // This is like saying: "I want to listen for connections on this address:port."

    struct sockaddr_in server_addr;

    // Always zero out structures before using them to ensure no garbage data.
    memset(&server_addr, 0, sizeof(server_addr));

    // Configure the address structure:
    server_addr.sin_family = AF_INET;  // IPv4

    // INADDR_ANY means "listen on all available network interfaces"
    // This could be 127.0.0.1 (localhost), 192.168.1.5 (LAN), etc.
    // htonl() = "host to network long" - converts byte order
    //
    // WHY htonl()? Different CPUs store multi-byte numbers differently:
    //   - Little-endian (x86): stores 0x12345678 as 78 56 34 12
    //   - Big-endian (network): stores 0x12345678 as 12 34 56 78
    // Network protocols use big-endian, so we convert with htonl().
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // htons() = "host to network short" - converts port number to network byte order
    server_addr.sin_port = htons(port);

    // bind() associates the socket with the address:port we configured.
    // Parameters:
    //   server_fd             - the socket to bind
    //   (struct sockaddr *)   - cast to generic socket address type
    //   sizeof(server_addr)   - size of the address structure
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    // After successful bind(), this port is reserved for this process.
    // No other process can bind to it until we close the socket.

    // ========================================================================
    // STEP 3: LISTEN FOR CONNECTIONS
    // ========================================================================
    // listen() marks the socket as a PASSIVE socket - one that will accept
    // incoming connection requests using accept().

    // listen() parameters:
    //   server_fd - the socket to mark as listening
    //   10        - backlog: max number of pending connections to queue
    //
    // The backlog is the queue size for connections that have completed
    // the TCP handshake but haven't been accept()ed yet. If this queue fills up,
    // new connections will be refused. 10 is a common default; busy servers
    // might use larger values.
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("Echo server listening on port %d...\n", port);

    // ========================================================================
    // STEP 4: ACCEPT AND HANDLE CONNECTIONS
    // ========================================================================
    // The server's main loop: accept connections and handle them one at a time.
    // This is a SEQUENTIAL server - it handles one client completely before
    // accepting the next one. See echo_server_threaded for concurrent handling.

    while (1) {  // Run forever
        // These variables will be filled in by accept() with the client's information.
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // accept() BLOCKS until a client connects.
        // It extracts the first connection from the pending queue (created by listen()).
        //
        // accept() parameters:
        //   server_fd     - the listening socket
        //   (struct sockaddr *)&client_addr - OUTPUT: client's address
        //   &client_len   - INPUT/OUTPUT: size of address structure
        //
        // accept() returns a NEW socket file descriptor for this specific client.
        // The original server_fd continues to listen for new connections.
        // This is a key concept: server_fd is for LISTENING, client_fd is for COMMUNICATION.
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;  // Try accepting the next connection
        }

        // At this point, we have a connected client!
        // client_addr contains the client's IP address and port number.

        // ====================================================================
        // CONVERT CLIENT IP ADDRESS TO HUMAN-READABLE STRING
        // ====================================================================
        // The client's IP is stored in binary form in client_addr.sin_addr.
        // inet_ntop() converts it to a string like "192.168.1.5".

        char client_ip[INET_ADDRSTRLEN];  // INET_ADDRSTRLEN = max length of IPv4 string

        // inet_ntop() = "network to presentation"
        // Parameters:
        //   AF_INET                  - address family (IPv4)
        //   &client_addr.sin_addr    - pointer to binary IP address
        //   client_ip                - buffer for the string
        //   sizeof(client_ip)        - size of buffer
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

        // ntohs() = "network to host short" - converts port from network to host byte order
        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // ====================================================================
        // STEP 5: ECHO LOOP - RECEIVE AND SEND BACK DATA
        // ====================================================================
        // We'll keep receiving data and echoing it back until the client disconnects.

        char buffer[1024];
        ssize_t bytes_received;

        // Loop while recv() returns a positive number (data received).
        // recv() returns 0 when the client closes the connection gracefully.
        while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            // We received some data! Echo it back to the client.

            // send() parameters:
            //   client_fd       - socket to send to
            //   buffer          - data to send
            //   bytes_received  - number of bytes to send
            //   0               - flags
            //
            // IMPORTANT: We're sending exactly the bytes we received.
            // We don't null-terminate or interpret the data as a string.
            // This is a true "echo" - we're just reflecting raw bytes.
            ssize_t bytes_sent = send(client_fd, buffer, bytes_received, 0);
            if (bytes_sent == -1) {
                perror("send");
                break;  // Exit the loop on send error
            }

            // In this simple example, we're not checking if bytes_sent equals
            // bytes_received. Production code should loop to ensure all data is sent.
        }

        // We exited the loop. Either:
        //   1. bytes_received == 0  (client closed connection)
        //   2. bytes_received == -1 (error occurred)
        if (bytes_received == -1) {
            perror("recv");
        }

        printf("Client %s disconnected.\n", client_ip);

        // ====================================================================
        // CLOSE THE CLIENT CONNECTION
        // ====================================================================
        // Always close the client socket when done.
        // This frees the file descriptor and sends a FIN packet to close the TCP connection.
        close(client_fd);

        // Now the server loops back to accept() to wait for the next client.
    }

    // This code is never reached (infinite loop), but in a real server you'd:
    close(server_fd);
    return 0;
}

// ============================================================================
// KEY SERVER CONCEPTS DEMONSTRATED:
// ============================================================================
//
// 1. SERVER VS CLIENT SOCKET SETUP:
//    Client: socket() -> connect() -> send()/recv() -> close()
//    Server: socket() -> bind() -> listen() -> accept() -> send()/recv() -> close()
//
// 2. TWO TYPES OF SOCKETS:
//    - Listening socket (server_fd): Waits for new connections, never sends/receives data
//    - Connected socket (client_fd): Communicates with a specific client
//
// 3. BYTE ORDER (ENDIANNESS):
//    - htonl() / htons(): Host to Network (Long/Short)
//    - ntohl() / ntohs(): Network to Host (Long/Short)
//    Always use these when working with addresses and ports!
//
// 4. BLOCKING ACCEPT:
//    accept() blocks until a client connects. This is why servers typically
//    either run in an infinite loop or use select/poll/epoll for multiple connections.
//
// 5. SEQUENTIAL PROCESSING:
//    This server handles one client at a time. While serving one client,
//    other clients must wait. See echo_server_threaded.c for concurrent handling.
//
// 6. GRACEFUL SHUTDOWN:
//    recv() returning 0 is the proper way to detect that the client closed
//    the connection. This is the TCP FIN packet being received.
//
// 7. ERROR RECOVERY:
//    Notice we 'continue' on accept() errors but 'break' on send() errors.
//    accept() errors are per-connection, so we can try the next one.
//    send() errors mean this connection is broken, so we close it.
//
// 8. SO_REUSEADDR:
//    Essential for development! Without it, you'd have to wait ~60 seconds
//    between server restarts. In production, it's still useful and safe.
//
// ============================================================================
