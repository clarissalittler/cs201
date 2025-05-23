/*
 * Exercise 1: Simple Echo Server
 * Difficulty: 🟢 Beginner
 * 
 * OBJECTIVE:
 * Create a basic TCP echo server that listens on port 8080 and echoes back
 * any message it receives from a client. This introduces the fundamental
 * socket API calls.
 * 
 * REQUIREMENTS:
 * 1. Create a TCP socket using socket()
 * 2. Bind the socket to port 8080 on localhost
 * 3. Listen for incoming connections
 * 4. Accept one client connection
 * 5. Read a message from the client and echo it back
 * 6. Close the connection and exit
 * 
 * EXPECTED BEHAVIOR:
 * Server: Listening on port 8080...
 * Server: Client connected
 * Server: Received: Hello, Server!
 * Server: Echoed back to client
 * 
 * Test with: nc localhost 8080
 * 
 * HINTS:
 * - Use socket(AF_INET, SOCK_STREAM, 0) for TCP
 * - Remember to set SO_REUSEADDR socket option
 * - Use struct sockaddr_in for address structure
 * - Don't forget to convert port to network byte order with htons()
 * 
 * COMMON PITFALLS:
 * - Forgetting to handle errors from system calls
 * - Not setting SO_REUSEADDR (leads to "Address already in use")
 * - Incorrect byte order for port number
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // TODO: Create a TCP socket
    
    // TODO: Set SO_REUSEADDR option to allow quick restart
    
    // TODO: Setup server address structure
    // - Set address family to AF_INET
    // - Set port to PORT (use htons())
    // - Set address to INADDR_ANY
    
    // TODO: Bind the socket to the address
    
    // TODO: Listen for connections (backlog of 1 is fine)
    
    printf("Server: Listening on port %d...\n", PORT);
    
    // TODO: Accept a client connection
    
    printf("Server: Client connected\n");
    
    // TODO: Read message from client
    
    // TODO: Echo the message back to client
    
    // TODO: Close client socket
    
    // TODO: Close server socket
    
    return 0;
}