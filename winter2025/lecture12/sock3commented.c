// PEDAGOGICAL PURPOSE:
// This program demonstrates a MULTI-CLIENT ECHO SERVER - handles multiple clients SEQUENTIALLY.
// Key learning objectives:
// 1. Understanding how to handle multiple clients sequentially (one after another)
// 2. Learning the nested loop pattern (outer loop for clients, inner loop for messages)
// 3. Understanding client limits and why we might want to limit connections
// 4. Seeing the difference between concurrent and sequential client handling
// 5. Understanding that this server can't handle clients simultaneously (blocking)
// 6. Learning how accept() creates a new socket for each client
// 7. Building on sock2.c by adding ability to serve multiple clients
// 8. Understanding server lifecycle management

#include <stdio.h>      // Provides input/output functions like printf
#include <stdlib.h>     // Provides general utilities like memory management
#include <string.h>     // Provides string manipulation functions like strlen, memset
#include <unistd.h>     // Provides UNIX system calls like close(), getpid()
#include <arpa/inet.h>  // Provides networking functions and structures for internet operations
#include <sys/stat.h>   // Provides file status structures (unused in this program)
#include <fcntl.h>      // Provides file control operations (unused in this program)

int main(){

  // UNUSED VARIABLE:
  // Declared but not used - likely leftover from experimentation
  struct stat fileStat;

  // SERVER CONFIGURATION:
  // Same as sock1.c and sock2.c
  int port = 8080;       // The port number the server will listen on

  // SOCKET FILE DESCRIPTORS:
  // Same as previous examples
  int server_fd;         // File descriptor for the server socket
  int new_socket;        // File descriptor for the accepted client connection

  // ADDRESS STRUCTURE:
  // Same as previous examples
  struct sockaddr_in address;  // Structure to hold server address information
  int addrlen = sizeof(address);  // Size of the address structure

  // MESSAGE BUFFER:
  // Same as sock2.c - used for receiving and echoing messages
  char buffer[1024];  // Buffer to hold messages

  // NEW FEATURE: CLIENT LIMIT
  // This is the KEY ADDITION to sock2.c
  //
  // maxClients: Maximum number of clients to serve
  //   - Server will handle 5 clients, then exit
  //   - Could be any number (10, 100, infinite with while(1))
  //
  // clients: Counter for how many clients we've served so far
  //   - Starts at 0
  //   - Incremented after each accept()
  //   - When clients >= maxClients, outer loop exits
  //
  // WHY LIMIT CLIENTS?
  //   - For testing/demonstration purposes
  //   - To prevent runaway servers in development
  //   - Real servers often have connection limits for resource management
  //   - Production servers usually run indefinitely (while(1))
  int maxClients = 5;
  int clients = 0;

  // STEP 1: CREATE SOCKET
  // Same as sock1.c and sock2.c
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Arguments:
  // - AF_INET: Use IPv4 internet protocol
  // - SOCK_STREAM: Use TCP protocol (reliable, ordered delivery)
  // - 0: Use default protocol (TCP for this combination)

  // STEP 2: CONFIGURE ADDRESS STRUCTURE
  // Same as previous examples
  address.sin_family = AF_INET;  // IPv4
  address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address
  address.sin_port = htons(port);  // Set port number, converting to network byte order

  // STEP 3: BIND SOCKET TO ADDRESS AND PORT
  // Same as previous examples
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));

  // Arguments:
  // - server_fd: The socket to bind
  // - (struct sockaddr *)&address: Cast address structure to generic socket address
  // - sizeof(address): Size of address structure

  // STEP 4: START LISTENING
  // Same as sock2.c
  listen(server_fd, 11);

  // Arguments:
  // - server_fd: Socket to listen on
  // - 11: Backlog - maximum length of queue for pending connections

  // INFORMATIVE OUTPUT:
  // Same as sock2.c - shows process ID and port
  printf("Server %d listening on port %d\n", getpid(), port);

  // NEW FEATURE: OUTER LOOP FOR MULTIPLE CLIENTS
  // This is the MAIN DIFFERENCE from sock2.c
  //
  // NESTED LOOP STRUCTURE:
  //   Outer loop (while clients < maxClients):
  //     - Handles DIFFERENT clients
  //     - Each iteration serves ONE client completely
  //     - Exits after serving maxClients clients
  //
  //   Inner loop (while(1)):
  //     - Handles MESSAGES from ONE client
  //     - Each iteration processes ONE message
  //     - Exits when client disconnects
  //
  // FLOW:
  //   1. Accept client 1
  //   2. Loop: handle all messages from client 1
  //   3. Client 1 disconnects
  //   4. Accept client 2
  //   5. Loop: handle all messages from client 2
  //   6. Client 2 disconnects
  //   ... (repeat until 5 clients served)
  //
  // IMPORTANT: This is SEQUENTIAL, not CONCURRENT
  //   - Server handles one client at a time
  //   - If client 1 is connected, client 2 must WAIT
  //   - Client 2 is in the accept queue (backlog)
  //   - Only after client 1 disconnects does server accept() client 2
  while(clients < maxClients){

    // STEP 5: ACCEPT A CLIENT CONNECTION
    // Each iteration of the outer loop accepts a NEW client
    //
    // IMPORTANT: This BLOCKS until a client connects
    //   - If no clients are waiting, server pauses here
    //   - When a client connects, accept() returns immediately
    //
    // new_socket VALUE:
    //   - Each accept() creates a NEW file descriptor
    //   - First client: new_socket might be 4
    //   - Second client: new_socket might be 4 again (after previous close)
    //   - Or could be different if other FDs are open
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    // INCREMENT CLIENT COUNTER:
    // Track how many clients we've accepted
    //
    // WHY INCREMENT HERE?
    //   - Right after accept() succeeds
    //   - Before entering inner loop
    //   - Ensures we count each client exactly once
    //
    // ALTERNATIVE PLACEMENT:
    //   - Could increment at end of inner loop (after client disconnects)
    //   - Current placement is clearer: count when client connects
    clients++;

    // INFORMATIVE OUTPUT:
    // Same typo as sock2.c: "lissstening"
    // Shows which socket (file descriptor) is handling this client
    printf("You are lissstening to %d:\n", new_socket);

    // INNER LOOP: HANDLE MESSAGES FROM THIS CLIENT
    // Same loop as sock2.c
    // Continues until client disconnects
    while(1) {

      // CLEAR BUFFER BEFORE READING:
      // Same as sock2.c - prevents old data contamination
      memset(buffer, 0, 1024);  // Clear buffer

      // READ DATA FROM CLIENT:
      // Same as sock2.c
      int valread = read(new_socket, buffer, 1024);  // Read message from client

      // PROCESS RECEIVED DATA:
      // Same logic as sock2.c
      if (valread > 0) {
        // CLIENT SENT DATA

        // Print received message to server console
        printf("Received: %s", buffer);  // Print received message

        // Echo message back to client
        write(new_socket, buffer, strlen(buffer));  // Echo message back to client
      }
      else {
        // CLIENT DISCONNECTED OR ERROR

        // Exit inner loop
        // Will proceed to close this client's socket
        // Then outer loop will accept next client
        break;  // Exit loop if no more data is received
      }
    }
    // INNER LOOP EXITS HERE when client disconnects
    // Now we'll close this client's socket and accept next client
  }
  // OUTER LOOP EXITS HERE after serving maxClients clients

  // CLEANUP: CLOSE THE LAST CLIENT SOCKET
  // This closes the socket for the LAST client (client #5)
  //
  // ISSUE: What about the other 4 client sockets?
  //   - They were closed implicitly when new_socket was reused
  //   - Actually, they're NOT closed properly!
  //   - This is a BUG - we should close(new_socket) after inner loop
  //
  // CORRECT APPROACH:
  //   while(clients < maxClients){
  //     new_socket = accept(...);
  //     clients++;
  //     while(1) { ... }
  //     close(new_socket);  // <-- Should be here!
  //   }
  //   // Don't need close(new_socket) here
  //
  // WHY IT STILL WORKS:
  //   - new_socket gets overwritten on next accept()
  //   - But this leaks file descriptors
  //   - With many clients, could run out of FDs
  close(new_socket);  // Close client socket

  // CLEANUP: CLOSE SERVER SOCKET
  // Same as previous examples
  // Shut down the server entirely
  close(server_fd);  // Close server socket

  return 0;
}

// EXPECTED OUTPUT (SERVER SIDE):
// Server 12345 listening on port 8080
// You are lissstening to 4:
// Received: Hello from client 1
// Received: Goodbye from client 1
// You are lissstening to 4:
// Received: Hello from client 2
// Received: Goodbye from client 2
// You are lissstening to 4:
// (continues for 5 clients total)
// (program exits after 5th client disconnects)

// TESTING WITH MULTIPLE CLIENTS:
//
// Terminal 1 (Server):
//   $ gcc sock3.c -o sock3
//   $ ./sock3
//   Server 12345 listening on port 8080
//
// Terminal 2 (Client 1):
//   $ telnet localhost 8080
//   Connected to localhost.
//   Hello
//   Hello
//   ^]
//   quit
//
// Terminal 3 (Client 2):
//   $ telnet localhost 8080
//   (waits if Client 1 is still connected)
//   (connects immediately after Client 1 disconnects)
//   Hi
//   Hi
//   ^]
//   quit
//
// Repeat with 3 more clients...

// THE NESTED LOOP PATTERN:
//
// Outer loop: Different clients
// while (have_capacity) {
//   accept_client();
//   Inner loop: Messages from one client
//   while (client_connected) {
//     read_message();
//     process_message();
//     send_response();
//   }
//   cleanup_client();
// }
//
// This pattern is common in simple servers

// KEY DIFFERENCES FROM sock2.c:
//
// 1. MULTIPLE CLIENTS:
//    - sock2.c: Serves 1 client then exits
//    - sock3.c: Serves 5 clients then exits
//
// 2. OUTER LOOP:
//    - sock2.c: No outer loop
//    - sock3.c: Outer loop with client counter
//
// 3. CLIENT TRACKING:
//    - sock2.c: No need to count clients
//    - sock3.c: clients variable tracks count
//
// 4. SERVER LIFETIME:
//    - sock2.c: Lives for one client connection
//    - sock3.c: Lives for five client connections

// SEQUENTIAL VS. CONCURRENT:
//
// SEQUENTIAL (this program):
//   - One client at a time
//   - Client 2 waits until Client 1 finishes
//   - Simple to implement
//   - Poor utilization if clients are slow
//   - Timeline:
//     Time 0: Client 1 connects
//     Time 1: Client 1 sends message
//     Time 2: Client 1 disconnects
//     Time 3: Client 2 connects  <-- Was waiting since time 0.5!
//     Time 4: Client 2 sends message
//
// CONCURRENT (requires threads/fork/select):
//   - Multiple clients simultaneously
//   - Client 2 connects while Client 1 is active
//   - Complex to implement
//   - Better utilization
//   - Timeline:
//     Time 0: Client 1 connects
//     Time 0.5: Client 2 connects (while Client 1 is still active)
//     Time 1: Both clients active
//     Time 2: Client 1 disconnects
//     Time 3: Client 2 still active

// WHAT HAPPENS TO WAITING CLIENTS?
//
// Scenario:
//   1. Client 1 connects and starts chatting
//   2. Client 2 tries to connect while Client 1 is active
//   3. accept() is blocked in the inner loop
//   4. Client 2 goes into the BACKLOG queue (remember listen(server_fd, 11))
//   5. When Client 1 disconnects, inner loop breaks
//   6. Outer loop iterates again
//   7. accept() immediately returns with Client 2 (no wait)
//   8. Client 2's connection was pending in the queue
//
// If MORE than 11 clients try to connect:
//   - First client is accepted and served
//   - Next 11 clients wait in backlog queue
//   - Client 12+ get "Connection refused"
//   - As each client finishes, next in queue is accepted

// COMMON MISTAKES:
//
// MISTAKE 1: Closing new_socket outside the loops
//   while(clients < maxClients) {
//     new_socket = accept(...);
//     while(1) { ... }
//     // Missing close(new_socket) here!
//   }
//   close(new_socket);  // Only closes last client
//   Result: File descriptor leak for all but last client
//   Fix: close(new_socket) after inner loop
//
// MISTAKE 2: Incrementing clients in wrong place
//   while(clients < maxClients) {
//     new_socket = accept(...);
//     while(1) {
//       ...
//       clients++;  // WRONG! Increments on every message
//     }
//   }
//   Result: Server exits after maxClients MESSAGES, not clients
//   Fix: Increment clients right after accept()
//
// MISTAKE 3: Not resetting buffer between clients
//   while(clients < maxClients) {
//     new_socket = accept(...);
//     // Missing memset(buffer, 0, 1024) here
//     while(1) { ... }
//   }
//   Result: First message from new client might have garbage
//   Fix: memset() is already in inner loop (correct placement)
//
// MISTAKE 4: Using clients++ as loop condition
//   while(clients++ < maxClients) { ... }
//   Result: Confusing, increments at wrong time
//   Fix: Keep increment separate and explicit
//
// MISTAKE 5: Thinking this handles concurrent clients
//   Many beginners think the outer loop allows concurrent clients
//   Reality: Still sequential - one client at a time
//   For concurrent: Need threads, fork(), or select()/poll()/epoll()

// LIMITATIONS OF THIS PROGRAM:
//
// 1. SEQUENTIAL CLIENT HANDLING:
//    - Can't serve multiple clients simultaneously
//    - Slow clients block fast clients
//    - Poor for real-world use
//
// 2. FIXED CLIENT LIMIT:
//    - Exits after 5 clients
//    - Real servers run indefinitely
//    - Should be while(1) in production
//
// 3. NO CONCURRENT CONNECTIONS:
//    - If Client 1 connects and never sends data, Client 2 is blocked
//    - No timeout mechanism
//    - Server can be "stuck" on a slow client
//
// 4. FILE DESCRIPTOR LEAK:
//    - Doesn't close client sockets properly (should close after inner loop)
//    - With many clients, could exhaust file descriptors
//
// 5. NO GRACEFUL SHUTDOWN:
//    - No way to stop server except killing the process
//    - Should handle signals (SIGINT, SIGTERM)

// HOW TO MAKE IT CONCURRENT:
//
// METHOD 1: FORK (Process per client)
//   while(1) {
//     new_socket = accept(...);
//     if (fork() == 0) {  // Child process
//       close(server_fd);
//       while(1) { /* handle client */ }
//       close(new_socket);
//       exit(0);
//     }
//     close(new_socket);  // Parent closes client socket
//   }
//
// METHOD 2: THREADS (Thread per client)
//   while(1) {
//     new_socket = accept(...);
//     pthread_create(&thread, NULL, handle_client, &new_socket);
//   }
//
// METHOD 3: SELECT/POLL (Event-driven)
//   while(1) {
//     select(max_fd, &readfds, ...);  // Wait for activity on any socket
//     if (FD_ISSET(server_fd, &readfds)) {
//       new_socket = accept(...);
//       add_to_fd_set(new_socket);
//     }
//     for (each client socket) {
//       if (FD_ISSET(client_fd, &readfds)) {
//         read_and_process(client_fd);
//       }
//     }
//   }

// WHEN TO USE SEQUENTIAL VS. CONCURRENT:
//
// USE SEQUENTIAL:
//   - Simple applications
//   - Low traffic
//   - Clients finish quickly
//   - Learning/prototyping
//
// USE CONCURRENT:
//   - High traffic
//   - Long-lived connections
//   - Multiple simultaneous clients expected
//   - Production servers

// TIMELINE EXAMPLE:
//
// Client 1 connects at t=0, sends 3 messages over 30 seconds, disconnects at t=30
// Client 2 tries to connect at t=5
// Client 3 tries to connect at t=10
//
// What happens:
//   t=0:  Client 1 connects, starts chatting
//   t=5:  Client 2 tries to connect, goes into backlog queue
//   t=10: Client 3 tries to connect, goes into backlog queue
//   t=30: Client 1 disconnects, inner loop breaks
//   t=30: accept() immediately returns Client 2 (was waiting in queue)
//   t=30: Client 2 starts chatting
//   (Client 3 still waiting)
//   ...
//
// With concurrent server:
//   t=0:  Client 1 connects, starts chatting
//   t=5:  Client 2 connects, starts chatting (simultaneously with Client 1)
//   t=10: Client 3 connects, starts chatting (simultaneously with 1 and 2)
//   All three chat at the same time!

// TRY IT:
// 1. Compile: gcc sock3.c -o sock3
// 2. Run server: ./sock3
// 3. In another terminal: telnet localhost 8080
// 4. Send a few messages but DON'T disconnect yet
// 5. In a THIRD terminal: telnet localhost 8080
//    Observe: Hangs waiting to connect (waiting for first client to finish)
// 6. Disconnect first client (Ctrl+] then quit)
// 7. Observe: Second client immediately connects
// 8. EXPERIMENT: Try connecting 10 clients at once (open 10 terminals)
//    See how they're served one at a time
// 9. EXPERIMENT: Change maxClients to 2, see server exit sooner
// 10. EXPERIMENT: Change maxClients check to while(1) for infinite server
// 11. CHALLENGE: Add close(new_socket) after inner loop
// 12. CHALLENGE: Print client number when each client connects
// 13. CHALLENGE: Track total messages from all clients
// 14. CHALLENGE: Add timestamp to each received message
