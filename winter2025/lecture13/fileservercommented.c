// PEDAGOGICAL PURPOSE:
// This program demonstrates a SOCKET-BASED FILE SERVER that sends file contents to clients.
// Key learning objectives:
// 1. Understanding how to serve files over sockets
// 2. Learning to open and read files with open() and read()
// 3. Understanding the server accept loop for multiple clients
// 4. Seeing practical integration of file I/O with socket I/O
// 5. Learning about potential security vulnerabilities (path traversal)

// file_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>       // For open()
#include <sys/stat.h>    // For file operations

const int port = 8080;
const int bsize = 1024;

int main() {
  int serverFd, newSocket;
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  char buffer[bsize];
  ssize_t bytesRead, bytesSent;

  // STEP 1: CREATE SERVER SOCKET
  serverFd = socket(AF_INET, SOCK_STREAM, 0);

  // STEP 2: CONFIGURE ADDRESS
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;  // Accept from any interface
  address.sin_port = htons(port);

  // STEP 3: BIND SOCKET TO PORT
  bind(serverFd, (struct sockaddr *)&address, addrlen);

  // STEP 4: START LISTENING
  listen(serverFd, 3);  // Backlog of 3 pending connections

  printf("Server listening on port %d...\n", port);

  // STEP 5: MAIN SERVER LOOP
  // Handle multiple clients sequentially
  while (1) {
    // ACCEPT CLIENT CONNECTION:
    // Blocks until a client connects
    newSocket = accept(serverFd, (struct sockaddr *)&address, &addrlen);

    // READ FILENAME FROM CLIENT:
    // Client sends filename as first message
    memset(buffer, 0, bsize);
    ssize_t filenameLen = read(newSocket, buffer, bsize - 1);
    buffer[filenameLen] = '\0'; // Null-terminate the filename

    printf("Client requested file: %s\n", buffer);

    // OPEN THE REQUESTED FILE:
    // open(filename, O_RDONLY)
    //   - Opens file for reading only
    //   - Returns file descriptor (fileFd)
    //   - Returns -1 if file doesn't exist or can't be opened
    //
    // SECURITY CONCERN:
    //   - Client controls filename
    //   - Could request "../../../etc/passwd"
    //   - This is a PATH TRAVERSAL VULNERABILITY
    //   - Production servers must validate/sanitize filename!
    int fileFd = open(buffer, O_RDONLY);

    // SEND FILE CONTENTS TO CLIENT:
    // Loop: read from file, write to socket
    //
    // WHY LOOP?
    //   - File might be larger than buffer
    //   - Read in chunks, send each chunk
    //   - Continue until end of file
    while ((bytesRead = read(fileFd, buffer, bsize)) > 0) {
      // READ FROM FILE:
      // bytesRead = number of bytes read from file
      // Could be less than bsize if near end of file

      // WRITE TO SOCKET:
      // Send exactly what we read
      bytesSent = write(newSocket, buffer, bytesRead);

      // NOTE: Should check bytesSent == bytesRead
      // Could have partial write (rare but possible)
    }

    // CLOSE FILE:
    // Done reading this file
    close(fileFd);

    // CLOSE CLIENT CONNECTION:
    // Closing socket sends EOF to client
    // Client knows file transfer is complete
    close(newSocket);

    printf("Connection closed.\n");
  }
  // Loop continues - server runs forever

  // UNREACHABLE (server runs indefinitely)
  close(serverFd);
  return 0;
}

// EXPECTED OUTPUT:
// Server listening on port 8080...
// Client requested file: test.txt
// Connection closed.
// Client requested file: data.txt
// Connection closed.

// FILE TRANSFER PROTOCOL:
//
// 1. Client connects to server
// 2. Client sends filename (string)
// 3. Server reads filename
// 4. Server opens file
// 5. Server reads file in chunks
// 6. Server sends each chunk to client
// 7. Server closes file
// 8. Server closes connection (signals EOF to client)
// 9. Client knows transfer is complete

// SECURITY VULNERABILITIES:
//
// PATH TRAVERSAL:
//   Client sends: "../../../etc/passwd"
//   Server opens and sends password file!
//   FIX: Validate filename, restrict to specific directory
//
// DENIAL OF SERVICE:
//   Client requests huge file
//   Server spends all resources serving it
//   FIX: Limit file size, rate limiting
//
// IMPROPER ERROR HANDLING:
//   File doesn't exist (fileFd = -1)
//   read() from invalid FD fails
//   Client receives nothing or error message
//   FIX: Check if open() succeeded, send error response

// IMPROVEMENTS NEEDED:
//
// 1. ERROR CHECKING:
//    if (fileFd < 0) {
//      write(newSocket, "ERROR: File not found\n", ...);
//      close(newSocket);
//      continue;
//    }
//
// 2. PATH VALIDATION:
//    if (strstr(filename, "..") != NULL) {
//      // Reject path traversal attempt
//    }
//
// 3. CHROOT JAIL:
//    chroot("/var/www/files");
//    // Server can only access files in this directory tree

// TRY IT:
// 1. Compile: gcc fileserver.c -o fileserver
// 2. Run: ./fileserver
// 3. Create test file: echo "Hello" > test.txt
// 4. In another terminal: ./fileclient test.txt
// 5. See file contents transferred
// 6. EXPERIMENT: Request /etc/hosts (if running as root)
// 7. EXPERIMENT: Request non-existent file
// 8. CHALLENGE: Add error handling for missing files
// 9. CHALLENGE: Limit file access to specific directory
// 10. CHALLENGE: Add file size limit
