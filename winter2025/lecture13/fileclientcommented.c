// PEDAGOGICAL PURPOSE:
// This program demonstrates a SOCKET-BASED FILE TRANSFER CLIENT.
// Key learning objectives:
// 1. Understanding how to use sockets for file transfer
// 2. Learning client-side socket operations (connect instead of bind/listen)
// 3. Understanding inet_pton() for converting IP addresses
// 4. Learning how to receive data in chunks over a socket
// 5. Seeing practical use of read() loop for variable-length data
// 6. Understanding localhost (127.0.0.1) and loopback interface

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>   // Provides socket functions and structures
#include <sys/socket.h>
#include <sys/types.h>

const int port = 8080;   // Server's port
const int bsize = 1024;  // Buffer size for reading file data

int main(int argc, char *argv[]) {
    int sock = 0;
    struct sockaddr_in servAddr;
    char buffer[bsize];
    char *filename = argv[1];  // Filename from command line argument

    // STEP 1: CREATE SOCKET
    // Same as server - create TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // STEP 2: CONFIGURE SERVER ADDRESS
    // Setting up address of the SERVER we want to connect to
    //
    // sin_family: IPv4
    servAddr.sin_family = AF_INET;

    // sin_port: Server's port (8080)
    // htons() converts to network byte order
    servAddr.sin_port = htons(port);

    // CONVERT IP ADDRESS STRING TO BINARY:
    // inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr)
    //   - pton = Presentation TO Network
    //   - Converts "127.0.0.1" string to binary format
    //   - 127.0.0.1 is LOCALHOST (loopback address)
    //   - Means "this same computer"
    //
    // WHY 127.0.0.1?
    //   - For testing: client and server on same machine
    //   - Production: Would use actual server IP
    //
    // ALTERNATIVE ADDRESSES:
    //   - "192.168.1.100" - local network server
    //   - "example.com" - would need DNS resolution first
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);

    // STEP 3: CONNECT TO SERVER
    // This is the KEY CLIENT OPERATION (different from server)
    //
    // connect(sock, address, size):
    //   - Initiates connection to server
    //   - Server must be listening on this address/port
    //   - Three-way TCP handshake happens here
    //   - Blocks until connection established or fails
    //
    // SERVER vs CLIENT:
    //   - Server: socket() → bind() → listen() → accept()
    //   - Client: socket() → connect()
    //   - Much simpler on client side!
    connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr));

    // STEP 4: SEND FILENAME TO SERVER
    // Tell server which file we want
    //
    // PROTOCOL:
    //   - Client sends filename
    //   - Server reads filename, opens file
    //   - Server sends file contents back
    //   - This is a simple request-response protocol
    write(sock, filename, strlen(filename)); // sending the filename

    // INFORM USER:
    printf("Contents of '%s':\n", filename);

    // STEP 5: RECEIVE FILE CONTENTS
    // Loop until all data received
    //
    // WHY LOOP?
    //   - File might be larger than buffer
    //   - TCP might split data into multiple packets
    //   - One read() call may not get all data
    //   - Must loop until EOF (server closes connection)
    while (1) {
      // READ CHUNK OF FILE:
      // read(sock, buffer, bsize - 1)
      //   - Reads up to (bsize - 1) bytes
      //   - bsize - 1 leaves room for null terminator
      //   - Returns number of bytes actually read
      ssize_t bytesReceived = read(sock, buffer, bsize - 1); // reading the file

      if (bytesReceived == 0) {
        // EOF: SERVER CLOSED CONNECTION
        // This means all data has been sent
        // Server closed socket after sending entire file
        // This is how we know file transfer is complete
        break;
      }

      // NULL-TERMINATE THE BUFFER:
      // Makes buffer a valid C string
      // Allows use of printf("%s")
      //
      // WHY bytesReceived?
      //   - Only that many bytes are valid
      //   - Rest of buffer contains old data
      //   - Place null terminator right after valid data
      buffer[bytesReceived] = '\0'; // Null-terminate the buffer

      // PRINT FILE CONTENTS:
      // Display the chunk we received
      // No newline - want continuous output
      printf("%s", buffer);
    }
    printf("\n");  // Final newline after all content

    // STEP 6: CLOSE SOCKET
    // Done with file transfer
    close(sock);
    return 0;
}

// EXPECTED OUTPUT:
// $ ./fileclient test.txt
// Contents of 'test.txt':
// Hello, this is a test file.
// Line 2 of the file.
// End of file.

// USAGE:
// ./fileclient <filename>
// Example: ./fileclient /etc/hosts

// CLIENT VS SERVER SOCKET OPERATIONS:
//
// SERVER                           CLIENT
// ------                           ------
// socket()                         socket()
// bind()                           (no bind)
// listen()                         (no listen)
// accept() [blocks]                connect() [initiates]
// read()/write()                   write()/read()
// close()                          close()

// WHY NO bind() ON CLIENT?
//
// - Server needs a known port (clients must know where to connect)
// - Client port doesn't matter (server responds to whatever port client uses)
// - OS automatically assigns ephemeral port to client (e.g., 52341)
// - Client: "I don't care what port I use, just connect me to server's port 8080"

// TRY IT:
// 1. Run file server: ./fileserver
// 2. Create test file: echo "Hello World" > test.txt
// 3. Run client: ./fileclient test.txt
// 4. See file contents printed
// 5. EXPERIMENT: Request a large file
// 6. EXPERIMENT: Request non-existent file (server behavior?)
// 7. CHALLENGE: Add error checking for failed connect()
// 8. CHALLENGE: Support downloading from remote IP (not just 127.0.0.1)
