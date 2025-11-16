// PEDAGOGICAL PURPOSE:
// This program demonstrates a TCP client using Berkeley sockets in C.
// Key learning objectives:
// 1. Understanding the client side of TCP socket programming
// 2. Learning the socket creation, connection, and communication flow
// 3. Understanding sockaddr_in structure for addressing
// 4. Learning about network byte order (htons) vs host byte order
// 5. Understanding blocking I/O in socket communication
// 6. Learning proper error handling in network programming
// 7. Understanding the read-write loop for client-server communication
// 8. Learning about connection teardown and resource cleanup
//
// CORE CONCEPT:
// A TCP client follows these steps:
// 1. CREATE a socket (endpoint for communication)
// 2. CONFIGURE the server address (IP and port)
// 3. CONNECT to the server
// 4. COMMUNICATE (read/write loop)
// 5. CLOSE the connection
//
// This is the CLIENT side - it initiates the connection to a server.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// CONFIGURATION CONSTANTS:
// These define the connection parameters
#define SERVER_PORT 8080        // Port number server is listening on
#define BUFFER_SIZE 1024        // Size of buffer for reading/writing data

// WHY 8080?
// Port 8080 is commonly used for development/testing
// Ports 0-1023 require root privileges
// Ports 1024-49151 are user ports (safe to use)

// WHY 1024 BYTES?
// Common buffer size balancing memory usage and efficiency
// Powers of 2 are conventional for buffer sizes
// Large enough for most text messages, small enough to not waste memory

int main() {
    // SOCKET FILE DESCRIPTOR:
    // In Unix, "everything is a file" - including network connections!
    // sockfd is a file descriptor that refers to our socket
    // We'll use it with read()/write() just like a regular file
    int sockfd;

    // SERVER ADDRESS STRUCTURE:
    // sockaddr_in is a structure that holds:
    // - Address family (IPv4 vs IPv6)
    // - IP address
    // - Port number
    // Think of this as "where we want to connect to"
    struct sockaddr_in serv_addr;

    // COMMUNICATION BUFFER:
    // This array holds data we read from or write to the socket
    // Must be large enough to hold expected messages
    char buffer[BUFFER_SIZE];

    // STEP 1: CREATE SOCKET
    // socket() creates an endpoint for communication
    //
    // PARAMETERS:
    // AF_INET: Address Family - Internet (IPv4)
    //          Use AF_INET6 for IPv6
    // SOCK_STREAM: Type - Stream socket (TCP)
    //              Use SOCK_DGRAM for UDP
    // 0: Protocol - 0 means choose default for this type (TCP for SOCK_STREAM)
    //
    // RETURN VALUE:
    // Returns a file descriptor (positive integer) on success
    // Returns -1 on error
    //
    // WHY TCP (SOCK_STREAM)?
    // - Reliable: Guarantees delivery and order
    // - Connection-oriented: Explicit connect/disconnect
    // - Bidirectional: Can read and write
    // Alternative: UDP (SOCK_DGRAM) is connectionless, unreliable, but faster
    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // PERROR: Prints error message describing why system call failed
        // Includes errno value in human-readable format
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // STEP 2: CONFIGURE SERVER ADDRESS
    // We need to tell the system WHERE to connect
    // This involves setting up the sockaddr_in structure

    // ZERO OUT THE STRUCTURE:
    // memset() fills serv_addr with zeros
    // WHY? Ensures no garbage values in any fields
    // Some systems have extra fields in sockaddr_in that we don't set
    // Configure server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));

    // SET ADDRESS FAMILY:
    // Must match what we used in socket() call
    // AF_INET = IPv4
    serv_addr.sin_family = AF_INET;

    // SET PORT NUMBER:
    // htons() = Host TO Network Short
    //
    // WHY htons()?
    // Different computers store multi-byte numbers differently:
    // - Little-endian: Least significant byte first (x86/x64)
    // - Big-endian: Most significant byte first (network standard)
    //
    // Network byte order is ALWAYS big-endian
    // htons() converts port number to network byte order
    //
    // Example: Port 8080 = 0x1F90
    // Little-endian host: 90 1F
    // Big-endian network: 1F 90
    // htons() does the conversion automatically
    serv_addr.sin_port   = htons(SERVER_PORT);

    // SET IP ADDRESS:
    // inet_pton() = Internet Presentation TO Network
    // Converts human-readable IP address to binary format
    //
    // PARAMETERS:
    // AF_INET: We're using IPv4
    // "127.0.0.1": IP address string (localhost/loopback)
    // &serv_addr.sin_addr: Where to store the result
    //
    // RETURN VALUE:
    // 1 on success
    // 0 if the address format is invalid
    // -1 on system error
    //
    // WHAT IS 127.0.0.1?
    // The loopback address - always refers to THIS computer
    // Traffic to 127.0.0.1 never leaves the machine
    // Perfect for testing client/server on same machine
    //
    // FOR REMOTE SERVERS:
    // Replace "127.0.0.1" with actual server IP
    // Or use a hostname with getaddrinfo()
    // Convert localhost (127.0.0.1) or use INADDR_ANY if on same machine
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // STEP 3: CONNECT TO SERVER
    // connect() attempts to establish a connection to the server
    //
    // PARAMETERS:
    // sockfd: Our socket file descriptor
    // (struct sockaddr*)&serv_addr: Server address (cast to generic sockaddr)
    // sizeof(serv_addr): Size of address structure
    //
    // WHAT HAPPENS:
    // 1. Client sends SYN packet to server
    // 2. Server responds with SYN-ACK
    // 3. Client sends ACK
    // 4. Connection established (TCP three-way handshake)
    //
    // RETURN VALUE:
    // 0 on success
    // -1 on error (server not listening, network down, etc.)
    //
    // THIS CALL BLOCKS:
    // Waits until connection succeeds or fails
    // May timeout if server doesn't respond
    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // CONNECTION ESTABLISHED!
    // At this point, the TCP connection is active
    // We can now read from and write to the socket
    printf("Connected to server. Type your responses below.\n");

    // STEP 4: COMMUNICATION LOOP
    // This loop handles the ongoing conversation with the server
    // Protocol: Server sends prompt, client responds
    // Communication loop
    while (1) {
        // CLEAR BUFFER:
        // Zero out buffer before each read
        // WHY? Prevents old data from previous iteration appearing
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);

        // READ FROM SERVER:
        // read() attempts to read data from the socket
        //
        // PARAMETERS:
        // sockfd: Socket to read from
        // buffer: Where to store received data
        // BUFFER_SIZE - 1: Maximum bytes to read (leave room for null terminator)
        //
        // RETURN VALUE:
        // Positive number: How many bytes were actually read
        // 0: Connection closed by peer (server disconnected)
        // -1: Error occurred
        //
        // THIS CALL BLOCKS:
        // Waits until data arrives or connection closes
        // Won't return until there's something to read
        //
        // WHY BUFFER_SIZE - 1?
        // We'll add a null terminator at buffer[valread]
        // This prevents buffer overflow
        // Read prompt from server
        int valread = read(sockfd, buffer, BUFFER_SIZE - 1);
        if (valread <= 0) {
            // DISCONNECT OR ERROR:
            // valread == 0 means server closed connection gracefully
            // valread < 0 means network error occurred
            // Could be a disconnect or error
            printf("Server disconnected or error occurred.\n");
            break;
        }

        // NULL-TERMINATE THE STRING:
        // Data from network is just bytes, not necessarily null-terminated
        // We add '\0' to make it a proper C string
        // This allows safe use with printf and string functions
        // Display prompt to user
        buffer[valread] = '\0';

        // DISPLAY SERVER'S MESSAGE:
        // Print exactly what server sent (typically a prompt or question)
        // %s means print as string
        // We use buffer as-is because server's message includes formatting
        printf("%s", buffer);

        // GET USER INPUT:
        // Now we need to read what the user wants to send back
        // We use a separate buffer to avoid overwriting server's message
        // Get user input
        char userInput[BUFFER_SIZE];

        // READ USER'S RESPONSE:
        // fgets() reads a line from stdin (keyboard)
        //
        // PARAMETERS:
        // userInput: Buffer to store input
        // BUFFER_SIZE: Maximum characters to read
        // stdin: Read from standard input (keyboard)
        //
        // RETURN VALUE:
        // Pointer to userInput on success
        // NULL on error or EOF (Ctrl+D)
        //
        // BEHAVIOR:
        // Reads until newline (\n) or buffer full
        // INCLUDES the newline in the result
        // Automatically null-terminates
        if (!fgets(userInput, BUFFER_SIZE, stdin)) {
            // EOF OR ERROR:
            // User pressed Ctrl+D or input stream closed
            // If input fails, close
            printf("Error reading input or EOF.\n");
            break;
        }

        // PREPARE TO SEND:
        // Calculate how many bytes to send
        // strlen() counts characters up to (but not including) null terminator
        // This INCLUDES the newline from fgets()
        // Send user input to server
        int toSend = strlen(userInput);

        // WRITE TO SERVER:
        // write() sends data through the socket
        //
        // PARAMETERS:
        // sockfd: Socket to write to
        // userInput: Data to send
        // toSend: Number of bytes to send
        //
        // RETURN VALUE:
        // Positive number: How many bytes were actually sent
        // -1: Error occurred
        //
        // PARTIAL WRITES:
        // write() might send fewer bytes than requested
        // For production code, should loop until all data sent
        // For this simple client, we assume it all goes through
        //
        // THIS CALL MAY BLOCK:
        // If network buffer is full, write() waits
        // Usually doesn't block for small messages
        if (write(sockfd, userInput, toSend) < 0) {
            perror("write");
            break;
        }

        // LOOP CONTINUES:
        // Go back to start of while loop
        // Wait for next message from server
    }

    // STEP 5: CLEANUP
    // Connection is closing (loop exited)
    // close() terminates the connection and frees the socket
    //
    // WHAT HAPPENS:
    // 1. Sends FIN packet to server
    // 2. Waits for server's FIN-ACK
    // 3. Connection fully closed (TCP teardown)
    // 4. Socket file descriptor becomes invalid
    //
    // ALWAYS CLOSE SOCKETS:
    // Prevents resource leaks
    // Notifies server that we're done
    // Cleanup
    close(sockfd);
    return 0;
}

// COMPLETE PROGRAM FLOW:
//
// 1. CREATE socket (get file descriptor)
// 2. CONFIGURE serv_addr (IP + port)
// 3. CONNECT to server (TCP handshake)
// 4. LOOP:
//    a. READ prompt from server
//    b. CHECK for disconnect
//    c. DISPLAY prompt to user
//    d. GET user's input
//    e. SEND input to server
// 5. CLOSE socket (TCP teardown)

// TCP CONNECTION LIFECYCLE:
//
// CLIENT STATE         SERVER STATE         ACTION
// ============         ============         ======
// CLOSED              LISTEN               Server waiting for connections
// SYN_SENT            LISTEN               Client calls connect(), sends SYN
// SYN_SENT            SYN_RECEIVED         Server receives SYN, sends SYN-ACK
// ESTABLISHED         SYN_RECEIVED         Client receives SYN-ACK, sends ACK
// ESTABLISHED         ESTABLISHED          Connection open, data can flow
// ...data exchange...
// FIN_WAIT_1          ESTABLISHED          Client calls close(), sends FIN
// FIN_WAIT_2          CLOSE_WAIT           Server receives FIN, sends ACK
// FIN_WAIT_2          LAST_ACK             Server calls close(), sends FIN
// TIME_WAIT           LAST_ACK             Client receives FIN, sends ACK
// TIME_WAIT           CLOSED               Server receives ACK, closes
// CLOSED              CLOSED               Client waits, then closes

// COMMON ERRORS AND CAUSES:
//
// "Connection refused":
// - Server is not running
// - Server is listening on different port
// - Firewall blocking connection
//
// "No route to host":
// - IP address unreachable
// - Network cable unplugged
// - Wrong network configuration
//
// "Connection timeout":
// - Server not responding
// - Network congestion
// - Server behind NAT/firewall
//
// "Broken pipe" (on write):
// - Server closed connection
// - Network disconnected
// - Server crashed

// PROTOCOL DESIGN NOTES:
//
// This client expects a specific protocol:
// 1. Server sends a prompt/question
// 2. Client responds with input
// 3. Repeat
//
// The protocol is TEXT-BASED:
// - Easy to debug (can read with eyes)
// - Works well for interactive applications
// - Not efficient for binary data
//
// ALTERNATIVES:
// - Binary protocol (faster, more complex)
// - JSON/XML (structured, widely compatible)
// - Protocol Buffers (efficient, strongly typed)

// BLOCKING I/O IMPLICATIONS:
//
// Both read() and write() are BLOCKING:
// - read() waits until data arrives
// - write() waits if network buffer full
//
// PROBLEMS:
// - Can't do other work while waiting
// - Can't handle multiple connections
// - UI freezes while waiting for network
//
// SOLUTIONS:
// - Non-blocking I/O (with select/poll/epoll)
// - Multithreading (separate thread for I/O)
// - Asynchronous I/O (callbacks/promises)

// BUFFER SIZE CONSIDERATIONS:
//
// BUFFER_SIZE = 1024 is adequate for:
// - Short text messages
// - Interactive prompts
// - Simple protocols
//
// MAY BE TOO SMALL FOR:
// - Large text responses
// - File transfers
// - Binary data
//
// TOO LARGE:
// - Wastes stack space
// - May exceed stack size limits
// - Better to use heap allocation (malloc)

// SECURITY CONSIDERATIONS:
//
// THIS CODE IS NOT SECURE:
// - No encryption (traffic visible on network)
// - No authentication (anyone can connect)
// - No input validation
// - Vulnerable to buffer overflow if server sends > BUFFER_SIZE
//
// FOR PRODUCTION:
// - Use TLS/SSL (encrypted connection)
// - Implement authentication
// - Validate all input
// - Use safe string functions

// TESTING THIS CLIENT:
//
// NEED A SERVER:
// This client requires a server running on port 8080
// The server should:
// 1. Accept connections
// 2. Send prompts
// 3. Read responses
// 4. Close gracefully
//
// EXAMPLE SERVER:
// See server_scaffold.c for a compatible server

// TRY IT:
// 1. Start server: ./server (in another terminal)
// 2. Compile client: gcc client.c -o client
// 3. Run client: ./client
// 4. Type responses to server prompts
// 5. Press Ctrl+D to disconnect
// 6. Observe "Server disconnected" message
//
// EXPERIMENTS:
// 1. Start client WITHOUT server - see "Connection refused"
// 2. Kill server during conversation - see disconnect handling
// 3. Send very long input (> 1024 chars) - what happens?
// 4. Connect multiple clients to same server
// 5. Change SERVER_PORT - must match server's port
// 6. Change IP to remote server (if you have one)
// 7. Use tcpdump/Wireshark to see TCP packets:
//    sudo tcpdump -i lo port 8080
