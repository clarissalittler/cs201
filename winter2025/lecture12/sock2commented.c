// PEDAGOGICAL PURPOSE:
// This program demonstrates an ECHO SERVER - it reads messages from clients and sends them back.
// Key learning objectives:
// 1. Understanding bidirectional communication with sockets
// 2. Learning to READ data from a socket using read()
// 3. Understanding the importance of buffer management with memset()
// 4. Learning about continuous communication loops with while(1)
// 5. Understanding why we check the return value of read() (valread)
// 6. Learning how to detect client disconnection (valread <= 0)
// 7. Building on sock1.c by adding read capability and looping
// 8. Understanding the echo server pattern

#include <stdio.h>      // Provides input/output functions like printf
#include <stdlib.h>     // Provides general utilities like memory management
#include <string.h>     // Provides string manipulation functions like strlen, memset
#include <unistd.h>     // Provides UNIX system calls like close(), getpid()
#include <arpa/inet.h>  // Provides networking functions and structures for internet operations
#include <sys/stat.h>   // Provides file status structures (unused in this program, but included)
#include <fcntl.h>      // Provides file control operations (unused in this program, but included)

int main(){

  // UNUSED VARIABLE:
  // This structure is declared but never used in this program
  // Likely left over from experimentation or planned functionality
  struct stat fileStat;

  // SERVER CONFIGURATION:
  // Same as sock1.c - listening on port 8080
  int port = 8080;       // The port number the server will listen on

  // SOCKET FILE DESCRIPTORS:
  // Same as sock1.c
  int server_fd;         // File descriptor for the server socket
  int new_socket;        // File descriptor for the accepted client connection

  // ADDRESS STRUCTURE:
  // Same as sock1.c
  struct sockaddr_in address;  // Structure to hold server address information
  int addrlen = sizeof(address);  // Size of the address structure

  // MESSAGE BUFFER:
  // KEY DIFFERENCE FROM sock1.c:
  //   - sock1.c initialized buffer with a message
  //   - sock2.c leaves buffer uninitialized (will be filled by read())
  //   - We'll receive data INTO this buffer from the client
  char buffer[1024];  // Buffer to hold messages

  // STEP 1: CREATE SOCKET
  // Same as sock1.c
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Arguments:
  // - AF_INET: Use IPv4 internet protocol
  // - SOCK_STREAM: Use TCP protocol (reliable, ordered delivery)
  // - 0: Use default protocol (TCP for this combination)

  // STEP 2: CONFIGURE ADDRESS STRUCTURE
  // Same as sock1.c
  address.sin_family = AF_INET;  // IPv4
  address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address
  address.sin_port = htons(port);  // Set port number, converting to network byte order

  // STEP 3: BIND SOCKET TO ADDRESS AND PORT
  // Same as sock1.c
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));

  // Arguments:
  // - server_fd: The socket to bind
  // - (struct sockaddr *)&address: Cast address structure to generic socket address
  // - sizeof(address): Size of address structure

  // STEP 4: START LISTENING
  // DIFFERENCE FROM sock1.c:
  //   - sock1.c had backlog of 3
  //   - sock2.c has backlog of 11
  //   - This is arbitrary - both work fine for simple examples
  listen(server_fd, 11);

  // Arguments:
  // - server_fd: Socket to listen on
  // - 11: Backlog - maximum length of queue for pending connections
  //       (Higher than sock1.c, but still arbitrary for this example)

  // INFORMATIVE OUTPUT:
  // KEY DIFFERENCE: Prints the server's process ID
  // WHY getpid()?
  //   - Useful for debugging when running multiple servers
  //   - Helps identify which server process is running
  //   - Necessary if you want to kill a specific server process
  printf("Server %d listening on port %d\n", getpid(), port);

  // STEP 5: ACCEPT A CLIENT CONNECTION
  // Same as sock1.c, but without the detailed explanation
  new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

  // INFORMATIVE OUTPUT:
  // Typo: "lissstening" instead of "listening"
  // Shows which socket (file descriptor number) is handling this client
  printf("You are lissstening to %d:\n", new_socket);

  // NEW FEATURE: CONTINUOUS COMMUNICATION LOOP
  // This is the MAIN DIFFERENCE from sock1.c
  //
  // sock1.c: Send one message and exit
  // sock2.c: Loop continuously, reading and echoing messages
  //
  // WHY while(1)?
  //   - Infinite loop - keeps server running
  //   - Continues until client disconnects or error occurs
  //   - Common pattern for network servers
  //   - Loop exits via 'break' when client disconnects
  while(1) {

    // CLEAR THE BUFFER BEFORE READING:
    // This is CRITICAL for correct operation
    //
    // memset(buffer, 0, 1024):
    //   - Sets all bytes in buffer to 0 (null bytes)
    //   - Ensures old data doesn't contaminate new reads
    //
    // WHY IS THIS NECESSARY?
    //   - Suppose first message is "Hello World" (11 bytes + newline)
    //   - Buffer now contains: "Hello World\n\0garbage..."
    //   - Second message is "Hi" (2 bytes + newline)
    //   - Without memset: "Hi\nlo World\n\0garbage..."
    //   - We'd echo back the OLD message mixed with the new one!
    //
    // MEMSET ARGUMENTS:
    //   - buffer: The memory to clear
    //   - 0: The value to set (null byte)
    //   - 1024: Number of bytes to clear
    //
    // ALTERNATIVE:
    //   - Could use bzero(buffer, 1024) - older UNIX function
    //   - Could manually: buffer[0] = '\0' - only clears first byte
    memset(buffer, 0, 1024);  // Clear buffer

    // READ DATA FROM THE CLIENT:
    // This is the CORE NEW FUNCTIONALITY
    //
    // read(new_socket, buffer, 1024):
    //   - Reads UP TO 1024 bytes from the client
    //   - Data is stored in buffer
    //   - Returns the number of bytes actually read
    //
    // WHY read() AND NOT scanf()?
    //   - scanf() reads from stdin (keyboard)
    //   - read() reads from any file descriptor (including sockets)
    //   - Sockets are file descriptors, so we use read()
    //
    // RETURN VALUE (valread):
    //   - POSITIVE: Number of bytes read (successful read)
    //   - ZERO: Client closed the connection gracefully (EOF)
    //   - NEGATIVE: Error occurred (network problem, invalid socket, etc.)
    //
    // IMPORTANT: read() MAY READ LESS THAN 1024 BYTES
    //   - If client sends 10 bytes, valread = 10
    //   - If client sends 2000 bytes, first read() gets 1024, need another read()
    //   - This is normal socket behavior - handle partial reads
    int valread = read(new_socket, buffer, 1024);  // Read message from client

    // CHECK IF WE RECEIVED DATA:
    // This if-else determines what to do based on read() result
    if (valread > 0) {
      // CLIENT SENT DATA - ECHO IT BACK

      // PRINT THE RECEIVED MESSAGE:
      // Shows what the client sent us
      // This prints to the SERVER'S console, not to the client
      printf("Received: %s", buffer);  // Print received message

      // ECHO THE MESSAGE BACK TO CLIENT:
      // Send the exact same data back
      //
      // WHY strlen(buffer)?
      //   - We only want to send the actual message
      //   - buffer is 1024 bytes, but message might be 10 bytes
      //   - strlen() finds the length of the string (up to null terminator)
      //   - Sends only the actual message, not the whole buffer
      //
      // POTENTIAL ISSUE:
      //   - If the message doesn't contain a null terminator, strlen() is unsafe
      //   - Better: Use valread (the number of bytes actually received)
      //   - Should be: write(new_socket, buffer, valread)
      //   - Current code works because most clients send null-terminated strings
      write(new_socket, buffer, strlen(buffer));  // Echo message back to client
    }
    else {
      // CLIENT DISCONNECTED OR ERROR OCCURRED

      // valread == 0: Client closed connection (graceful shutdown)
      //   - Client sent TCP FIN packet
      //   - This is normal - client is done
      //
      // valread < 0: Error occurred
      //   - Network problem
      //   - Socket closed unexpectedly
      //   - Should check errno for specific error
      //
      // RESPONSE: EXIT THE LOOP
      //   - No point continuing if client is gone
      //   - Break out of while(1) loop
      //   - Will proceed to cleanup code
      break;  // Exit loop if no more data is received
    }
  }

  // INCOMPLETE COMMENT:
  // This comment suggests there was planned functionality that wasn't implemented
  // Possibly reading a file or additional features
  /* need to fill in the */

  // CLEANUP: CLOSE THE CLIENT SOCKET
  // Same as sock1.c
  // Release resources for this client connection
  close(new_socket);  // Close client socket

  // CLEANUP: CLOSE THE SERVER SOCKET
  // Same as sock1.c
  // Shut down the server entirely
  close(server_fd);  // Close server socket

  return 0;
}

// EXPECTED OUTPUT (SERVER SIDE):
// Server 12345 listening on port 8080
// You are lissstening to 4:
// Received: Hello
// Received: How are you?
// Received: Goodbye
// (client disconnects, program exits)

// EXPECTED BEHAVIOR (CLIENT SIDE):
// Client connects
// Client sends: Hello
// Client receives: Hello
// Client sends: How are you?
// Client receives: How are you?
// Client sends: Goodbye
// Client receives: Goodbye
// Client disconnects

// TESTING THE ECHO SERVER:
//
// Terminal 1 (Server):
//   $ gcc sock2.c -o sock2
//   $ ./sock2
//   Server 12345 listening on port 8080
//
// Terminal 2 (Client using telnet):
//   $ telnet localhost 8080
//   Trying 127.0.0.1...
//   Connected to localhost.
//   Escape character is '^]'.
//   Hello
//   Hello
//   Test message
//   Test message
//   ^]
//   telnet> quit
//
// Terminal 2 (Client using nc):
//   $ nc localhost 8080
//   Hello
//   Hello
//   Test
//   Test
//   ^C

// THE ECHO SERVER PATTERN:
//
// 1. Accept client connection
// 2. Loop:
//    a. Read data from client
//    b. If data received, echo it back
//    c. If no data (client disconnected), exit loop
// 3. Close connection
//
// This pattern is fundamental to many network protocols

// KEY DIFFERENCES FROM sock1.c:
//
// 1. BIDIRECTIONAL COMMUNICATION:
//    - sock1.c: Server only sends
//    - sock2.c: Server receives AND sends
//
// 2. CONTINUOUS COMMUNICATION:
//    - sock1.c: Send once and exit
//    - sock2.c: Loop to handle multiple messages
//
// 3. CLIENT DISCONNECTION HANDLING:
//    - sock1.c: No need to detect disconnection
//    - sock2.c: Checks valread to detect when client leaves
//
// 4. BUFFER MANAGEMENT:
//    - sock1.c: Buffer initialized with message
//    - sock2.c: Buffer cleared before each read

// COMMON MISTAKES:
//
// MISTAKE 1: Not clearing buffer before read
//   // No memset
//   read(new_socket, buffer, 1024);
//   Result: Old data contaminates new messages
//   Fix: Always memset(buffer, 0, size) before reading
//
// MISTAKE 2: Using strlen() on non-null-terminated data
//   read(new_socket, buffer, 1024);
//   write(new_socket, buffer, strlen(buffer));  // DANGEROUS!
//   Problem: If buffer has no null terminator, strlen() reads past buffer
//   Fix: Use valread: write(new_socket, buffer, valread);
//
// MISTAKE 3: Not checking read() return value
//   read(new_socket, buffer, 1024);
//   write(new_socket, buffer, strlen(buffer));  // Might write garbage
//   Problem: If read() returns 0 or -1, buffer has no valid data
//   Fix: Check if (valread > 0) before using buffer
//
// MISTAKE 4: Exiting on first error without cleanup
//   if (valread < 0) exit(1);  // WRONG!
//   Problem: Sockets remain open, resources leak
//   Fix: close() sockets before exiting
//
// MISTAKE 5: Infinite loop without exit condition
//   while(1) {
//     read(...);
//     write(...);
//     // No break statement
//   }
//   Problem: Server never exits even when client disconnects
//   Fix: Break when valread <= 0

// LIMITATIONS OF THIS PROGRAM:
//
// 1. HANDLES ONLY ONE CLIENT:
//    - After first client disconnects, server exits
//    - Can't handle multiple clients concurrently
//    - Can't accept a second client after first one connects
//
// 2. SEQUENTIAL PROCESSING:
//    - Server blocks on read() waiting for client
//    - Can't do anything else while waiting
//    - Can't handle multiple clients simultaneously
//
// 3. NO ERROR HANDLING:
//    - Doesn't check if socket operations fail
//    - Doesn't distinguish between clean disconnect and error
//    - Should check errno when valread < 0
//
// 4. BUFFER OVERFLOW RISK:
//    - If client sends more than 1024 bytes in one message, we only get first 1024
//    - Should handle partial reads
//    - Should loop to read all data
//
// 5. NO PROTOCOL:
//    - Just echoes raw bytes
//    - Real applications need message framing
//    - Need to know where one message ends and next begins

// IMPROVEMENTS IN sock3.c:
//
// sock3.c adds:
//   - Outer loop to handle MULTIPLE clients sequentially
//   - Server continues running after a client disconnects
//   - Can accept up to maxClients clients

// WHY IS ECHO SERVER USEFUL?
//
// 1. TESTING:
//    - Simple way to test network connectivity
//    - Can verify data transmission works
//
// 2. LEARNING:
//    - Demonstrates bidirectional communication
//    - Shows the basics of reading and writing
//
// 3. DEBUGGING:
//    - Can use to test client code
//    - See exactly what client sends
//
// 4. PROTOCOL FOUNDATION:
//    - Many protocols are request-response (similar to echo)
//    - HTTP, FTP, SMTP all follow similar patterns

// MEMORY LAYOUT AFTER read():
//
// Suppose client sends: "Hello\n" (6 bytes)
//
// BEFORE memset:
// buffer: [g][a][r][b][a][g][e][...] (1024 bytes of random data)
//
// AFTER memset:
// buffer: [0][0][0][0][0][0][0][...] (1024 bytes of zeros)
//
// AFTER read:
// buffer: [H][e][l][l][o][\n][0][...] (6 bytes from client, rest still zero)
// valread = 6
//
// AFTER printf:
// Prints: "Hello\n"
//
// AFTER write:
// Sends to client: [H][e][l][l][o][\n] (6 bytes, using strlen)

// THE READ-WRITE LOOP PATTERN:
//
// This pattern appears in many network applications:
//
// while (connected) {
//   1. Clear buffer
//   2. Read from client
//   3. Check if still connected
//   4. Process data
//   5. Send response
// }
//
// Examples:
// - Echo server (this program)
// - HTTP server (read request, send response)
// - Chat server (read message, broadcast to all)
// - Game server (read player action, send game state)

// TCP GUARANTEES RELEVANT HERE:
//
// 1. ORDERED DELIVERY:
//    - If client sends "A", then "B", server receives "A", then "B"
//    - Never out of order
//
// 2. RELIABLE DELIVERY:
//    - Data won't be lost in transit
//    - If write() succeeds, data will reach the other side
//
// 3. CONNECTION-ORIENTED:
//    - Both sides know when connection is established
//    - Both sides know when connection is closed (valread = 0)
//
// 4. STREAM-BASED:
//    - No message boundaries
//    - If client sends "Hello" then "World", server might receive "HelloWorld"
//    - Application must handle message framing

// TRY IT:
// 1. Compile: gcc sock2.c -o sock2
// 2. Run server: ./sock2
// 3. In another terminal: telnet localhost 8080
// 4. Type messages and see them echoed back
// 5. Press Ctrl+] then type "quit" to exit telnet
// 6. Observe server exits after client disconnects
// 7. EXPERIMENT: Run server, connect, type 5 messages, disconnect
// 8. EXPERIMENT: What happens if you send a very long message (>1024 chars)?
// 9. EXPERIMENT: Connect but don't send anything - server waits
// 10. EXPERIMENT: Print valread to see how many bytes received
// 11. CHALLENGE: Modify to use valread instead of strlen() in write()
// 12. CHALLENGE: Add error checking - print "Error" if valread < 0
// 13. CHALLENGE: Count and print the total number of messages received
// 14. CHALLENGE: Instead of echoing, convert message to uppercase before sending
