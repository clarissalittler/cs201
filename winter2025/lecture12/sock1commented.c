// PEDAGOGICAL PURPOSE:
// This program demonstrates the BASICS of creating a TCP SERVER using sockets.
// Key learning objectives:
// 1. Understanding the four fundamental steps to create a TCP server (socket, bind, listen, accept)
// 2. Learning how to set up a sockaddr_in structure for IPv4 networking
// 3. Understanding the difference between server_fd (listening socket) and new_socket (client connection)
// 4. Learning about network byte order conversion with htons()
// 5. Understanding that sockets are file descriptors - we can read/write to them
// 6. Seeing the minimal code needed for a working server
// 7. Understanding why servers need to bind to a port and listen

#include <stdio.h>      // Provides input/output functions like printf
#include <stdlib.h>     // Provides general utilities like memory management
#include <string.h>     // Provides string manipulation functions like strlen
#include <unistd.h>     // Provides UNIX system calls like close()
#include <arpa/inet.h>  // Provides networking functions and structures for internet operations

int main(){

  // PORT NUMBER:
  // The port number the server will listen on
  // WHY 8080?
  //   - Ports below 1024 require root privileges
  //   - 8080 is a common alternative to port 80 (HTTP)
  //   - It's high enough to not need special permissions
  //   - It's commonly used for development web servers
  int port = 8080;

  // FILE DESCRIPTORS:
  // In UNIX, everything is a file - including network connections!
  // These integers represent "handles" to our sockets
  //
  // server_fd: The LISTENING socket
  //   - This socket WAITS for incoming connections
  //   - It doesn't communicate with any specific client
  //   - Think of it as the "reception desk" that receives visitors
  //
  // new_socket: The CLIENT CONNECTION socket
  //   - This socket represents a CONNECTION to a specific client
  //   - We use this to send/receive data with one client
  //   - Think of it as a "dedicated phone line" to one specific client
  int server_fd;         // File descriptor for the server socket
  int new_socket;        // File descriptor for the accepted client connection

  // ADDRESS STRUCTURE:
  // This structure holds all the information about the server's address
  // It tells the socket WHERE to listen and HOW to communicate
  //
  // WHY A STRUCTURE?
  //   - We need to specify: protocol family, IP address, and port
  //   - The structure groups these related pieces of information
  //   - Different socket types use different address structures
  struct sockaddr_in address;  // Structure to hold server address information

  // SIZE OF ADDRESS:
  // We need to pass the size of the address structure to several functions
  // This is a common C pattern for functions that work with variable-sized structures
  int addrlen = sizeof(address);  // Size of the address structure

  // MESSAGE BUFFER:
  // This buffer holds the message we'll send to clients
  // WHY 1024?
  //   - It's a common buffer size (powers of 2 are typical in computing)
  //   - 1024 bytes = 1 KB, enough for most small messages
  //   - We initialize it with our greeting message
  char buffer[1024] = "Hey there y'all\n";  // Message buffer with greeting

  // STEP 1: CREATE THE SOCKET
  // This is like getting a phone installed - we have the capability to communicate
  // but we haven't set up the phone number or started listening yet
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  // SOCKET ARGUMENTS BREAKDOWN:
  // AF_INET: Address Family - Internet (IPv4)
  //   - AF stands for "Address Family"
  //   - INET stands for "Internet"
  //   - This means we're using IPv4 addresses (like 192.168.1.1)
  //   - Alternative: AF_INET6 for IPv6
  //
  // SOCK_STREAM: Socket Type - TCP (Transmission Control Protocol)
  //   - STREAM means ordered, reliable, bidirectional communication
  //   - TCP guarantees: data arrives in order, no duplicates, no losses
  //   - Like a phone call: continuous stream of conversation
  //   - Alternative: SOCK_DGRAM for UDP (like sending postcards)
  //
  // 0: Protocol - Use default protocol for this combination
  //   - For AF_INET + SOCK_STREAM, the default is TCP
  //   - We could explicitly specify IPPROTO_TCP, but 0 is conventional
  //
  // RETURN VALUE:
  //   - Returns a file descriptor (positive integer) on success
  //   - Returns -1 on failure
  //   - We should check for errors, but this example omits error handling for clarity

  // STEP 2: CONFIGURE THE ADDRESS STRUCTURE
  // We're setting up all the details about WHERE the server will listen

  // Set the address family to IPv4
  // This MUST match the AF_INET we used in socket()
  address.sin_family = AF_INET;  // IPv4

  // Set the IP address to INADDR_ANY
  // WHAT IS INADDR_ANY?
  //   - Special constant meaning "any available network interface"
  //   - If the machine has multiple IP addresses (WiFi, Ethernet, localhost)
  //     the server will listen on ALL of them
  //   - Clients can connect via any of these addresses
  //   - Alternative: bind to a specific IP like inet_addr("127.0.0.1")
  //
  // WHY INADDR_ANY?
  //   - Maximum flexibility - works regardless of network configuration
  //   - Server accepts connections from anywhere
  //   - Common for servers that should be publicly accessible
  address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address

  // Set the port number with NETWORK BYTE ORDER conversion
  // WHAT IS htons()?
  //   - Host TO Network Short
  //   - Converts a 16-bit number from host byte order to network byte order
  //   - Byte order: the sequence in which bytes are stored in memory
  //
  // WHY DO WE NEED THIS?
  //   - Different CPUs store multi-byte numbers differently:
  //     - Intel (x86): Little-endian (least significant byte first)
  //     - Some others: Big-endian (most significant byte first)
  //   - Network protocols standardize on big-endian (network byte order)
  //   - htons() ensures our port number is in the correct format
  //
  // EXAMPLE:
  //   - Port 8080 in decimal
  //   - In hex: 0x1F90
  //   - Little-endian (host): 90 1F
  //   - Big-endian (network): 1F 90
  //   - htons() converts host → network automatically
  address.sin_port = htons(port);  // Set port number, converting to network byte order

  // STEP 3: BIND THE SOCKET TO THE ADDRESS AND PORT
  // This is like registering a phone number - we're claiming this port
  // After binding, no other process can use this port on this machine
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));

  // BIND ARGUMENTS BREAKDOWN:
  // server_fd: The socket file descriptor we created
  //
  // (struct sockaddr *)&address: The address to bind to
  //   - &address: Address of our sockaddr_in structure
  //   - (struct sockaddr *): Cast to generic socket address type
  //   - WHY THE CAST?
  //     - bind() is a generic function that works with different address types
  //     - It expects a generic sockaddr pointer
  //     - We have a specific sockaddr_in (for IPv4)
  //     - The cast tells the compiler "trust me, this is compatible"
  //
  // sizeof(address): Size of the address structure
  //   - bind() needs to know how much data to read
  //   - Different address types have different sizes
  //
  // RETURN VALUE:
  //   - Returns 0 on success
  //   - Returns -1 on failure (e.g., port already in use, permission denied)
  //   - Common error: "Address already in use" if another program is using port 8080

  // STEP 4: START LISTENING FOR CONNECTIONS
  // This is like turning on the phone ringer - we're ready to receive calls
  // The socket is now ready to accept incoming connection requests
  listen(server_fd, 3);

  // LISTEN ARGUMENTS BREAKDOWN:
  // server_fd: The socket to listen on
  //
  // 3: Backlog - maximum length of queue for PENDING connections
  //   - WHAT IS BACKLOG?
  //     - When clients try to connect, they might arrive faster than we can accept()
  //     - The backlog is the waiting room for these pending connections
  //     - If more than 3 clients try to connect before we accept(), extras are refused
  //   - WHY 3?
  //     - Just an example value
  //     - For busy servers, this might be 100 or more
  //     - For simple examples, 3 is sufficient
  //   - IMPORTANT: This is PENDING connections, not ACTIVE connections
  //     - We can have many active connections simultaneously
  //     - The backlog only affects clients waiting to be accept()ed
  //
  // RETURN VALUE:
  //   - Returns 0 on success
  //   - Returns -1 on failure

  // NOTIFY USER:
  // Print a message so we know the server is ready
  // This is important because accept() will BLOCK until a client connects
  printf("Server listening on port %d\n", port);

  // STEP 5: ACCEPT A CONNECTION
  // This is like answering the phone - we're establishing a conversation with a client
  // accept() BLOCKS (waits) until a client connects
  new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

  // ACCEPT ARGUMENTS BREAKDOWN:
  // server_fd: The listening socket to accept connections on
  //
  // (struct sockaddr *)&address: Will store connected client's address info
  //   - After accept() returns, this will contain the CLIENT's IP and port
  //   - We can use this to know WHO connected
  //   - The cast is needed for the same reason as in bind()
  //
  // (socklen_t*)&addrlen: Pointer to size of address structure
  //   - IN: We set this to sizeof(address) to tell accept() how much space we have
  //   - OUT: accept() updates this with the actual size used
  //   - The cast is needed because addrlen is int, but accept() expects socklen_t*
  //
  // RETURN VALUE:
  //   - Returns a NEW file descriptor for the client connection (new_socket)
  //   - This is DIFFERENT from server_fd
  //   - server_fd continues to listen for MORE clients
  //   - new_socket is used to communicate with THIS specific client
  //   - Returns -1 on failure
  //
  // BLOCKING BEHAVIOR:
  //   - accept() WAITS here until a client connects
  //   - The program is PAUSED at this line until a connection arrives
  //   - This is called "blocking I/O"
  //   - Alternative: non-blocking sockets (more advanced)

  // STEP 6: SEND DATA TO THE CLIENT
  // Now that we have a connection, we can send our message
  write(new_socket, buffer, strlen(buffer));  // Send message to client

  // WRITE ARGUMENTS BREAKDOWN:
  // new_socket: The file descriptor for THIS client connection
  //   - NOT server_fd (which is still listening)
  //   - This is the socket returned by accept()
  //
  // buffer: Pointer to the data to send
  //   - Our message: "Hey there y'all\n"
  //
  // strlen(buffer): Number of bytes to send
  //   - strlen() returns the length of the string (not including null terminator)
  //   - We only send the actual message, not the extra buffer space
  //   - strlen("Hey there y'all\n") = 16 bytes
  //
  // WHY write() AND NOT printf()?
  //   - write() sends data to a file descriptor
  //   - printf() sends data to stdout (the console)
  //   - Sockets are file descriptors, so we use write()
  //   - This is a key UNIX principle: everything is a file
  //
  // RETURN VALUE:
  //   - Returns the number of bytes actually written
  //   - Returns -1 on failure
  //   - In practice, we should check this, but example omits for simplicity

  // STEP 7: CLOSE THE CLIENT SOCKET
  // We're done talking to this client, so we close the connection
  // This is like hanging up the phone
  close(new_socket);  // Close client socket

  // WHY CLOSE new_socket?
  //   - Releases system resources (file descriptor)
  //   - Tells the client we're done (sends TCP FIN packet)
  //   - Good practice to close connections when finished
  //   - If we don't close, the socket remains open until the program exits

  // STEP 8: CLOSE THE SERVER SOCKET
  // We're shutting down the server entirely
  close(server_fd);  // Close server socket

  // WHY CLOSE server_fd?
  //   - Releases the port (other programs can now use port 8080)
  //   - Releases system resources
  //   - In a real server, we wouldn't close this - we'd keep listening
  //   - This example is a "one-shot" server that handles one client then exits

  return 0;
}

// EXPECTED OUTPUT:
// Server listening on port 8080
// (program waits here)
// (when client connects, message is sent)
// (program exits)

// TESTING THE SERVER:
// Terminal 1:
//   $ gcc sock1.c -o sock1
//   $ ./sock1
//   Server listening on port 8080
//
// Terminal 2:
//   $ telnet localhost 8080
//   Trying 127.0.0.1...
//   Connected to localhost.
//   Escape character is '^]'.
//   Hey there y'all
//   Connection closed by foreign host.

// THE SOCKET LIFECYCLE:
//
// 1. socket() - Create socket
//    ↓
// 2. bind() - Assign address and port
//    ↓
// 3. listen() - Start listening for connections
//    ↓
// 4. accept() - Accept a client connection (BLOCKS)
//    ↓
// 5. write() - Send data to client
//    ↓
// 6. close(new_socket) - Close client connection
//    ↓
// 7. close(server_fd) - Close server socket

// WHY TWO SOCKETS?
//
// server_fd (LISTENING SOCKET):
//   - Purpose: Wait for new client connections
//   - Bound to a specific port
//   - Never sends/receives application data
//   - Like a reception desk
//
// new_socket (CLIENT CONNECTION):
//   - Purpose: Communicate with ONE specific client
//   - Created by accept()
//   - Used for actual data transfer
//   - Like a dedicated phone line
//
// ANALOGY:
//   - server_fd is like the main phone number of a business
//   - new_socket is like transferring the call to a specific employee
//   - The main number keeps accepting new calls
//   - Each employee handles one customer

// COMMON MISTAKES:
//
// MISTAKE 1: Using wrong socket for communication
//   write(server_fd, buffer, strlen(buffer));  // WRONG!
//   The server_fd is for LISTENING, not for sending data
//   Always use new_socket (returned by accept()) for communication
//
// MISTAKE 2: Forgetting htons() for port
//   address.sin_port = port;  // WRONG!
//   On some systems this works by luck, but it's not portable
//   Always use htons(port) for correct byte order
//
// MISTAKE 3: Not casting address in bind() and accept()
//   bind(server_fd, &address, sizeof(address));  // WRONG!
//   Compiler error: incompatible pointer types
//   Must cast: (struct sockaddr *)&address
//
// MISTAKE 4: Using sizeof() on buffer pointer
//   write(new_socket, buffer, sizeof(buffer));  // POTENTIALLY WRONG!
//   If buffer is declared as char*, sizeof(buffer) = 8 (pointer size)
//   Use strlen(buffer) for strings, or track the actual data length
//
// MISTAKE 5: Port already in use
//   If you run the program twice:
//   bind: Address already in use
//   Solution: Wait a minute, or use SO_REUSEADDR socket option
//
// MISTAKE 6: Connecting before server is ready
//   Client gets "Connection refused"
//   Make sure server is running and has printed "listening" message

// LIMITATIONS OF THIS PROGRAM:
//
// 1. HANDLES ONLY ONE CLIENT:
//    - After the first client connects and disconnects, the server exits
//    - A real server would loop and accept multiple clients
//
// 2. NO ERROR HANDLING:
//    - All socket functions can fail, but we don't check return values
//    - Production code should check for errors and handle them
//
// 3. NO SECURITY:
//    - Anyone can connect from anywhere
//    - No authentication or encryption
//
// 4. BLOCKS ON ACCEPT:
//    - Server can't do anything else while waiting for a client
//    - Can't handle multiple clients simultaneously
//    - Solutions: threads, fork(), or non-blocking I/O

// IMPROVEMENTS IN sock2.c AND sock3.c:
//
// sock2.c adds:
//   - Reading data FROM the client (not just sending)
//   - Echo server functionality (send back what client sends)
//   - Loop to handle multiple messages from one client
//
// sock3.c adds:
//   - Loop to handle MULTIPLE clients sequentially
//   - Server doesn't exit after first client

// KEY CONCEPTS TO UNDERSTAND:
//
// 1. Sockets are file descriptors
//    - Can use read(), write(), close() just like files
//
// 2. Client-server architecture
//    - Server waits for clients
//    - Client initiates connection
//
// 3. TCP is connection-oriented
//    - Must establish connection before sending data
//    - Reliable, ordered delivery
//
// 4. Network byte order matters
//    - Always use htons() for ports
//    - Always use htonl() for IP addresses (if not using INADDR_ANY)
//
// 5. Two sockets: listening vs. connection
//    - One socket listens for new clients
//    - Separate socket for each client connection

// TRY IT:
// 1. Compile: gcc sock1.c -o sock1
// 2. Run server: ./sock1
// 3. In another terminal, connect: telnet localhost 8080
// 4. Observe the message printed by telnet
// 5. EXPERIMENT: Change the message in buffer
// 6. EXPERIMENT: Change the port number
// 7. EXPERIMENT: What happens if you run two servers on the same port?
// 8. EXPERIMENT: What happens if you try to connect before starting the server?
// 9. EXPERIMENT: Use nc (netcat) instead of telnet: nc localhost 8080
// 10. CHALLENGE: Modify to print the client's IP address after accept()
//     Hint: address contains client info after accept() returns
//     Use inet_ntoa(address.sin_addr) to convert IP to string
// 11. CHALLENGE: Add a loop to handle 5 clients before exiting
// 12. CHALLENGE: Add error checking for all socket functions
