/*
 * Exercise 2: TCP Client
 * Difficulty: 🟢 Beginner
 * 
 * OBJECTIVE:
 * Create a TCP client that connects to a server, sends a message,
 * receives a response, and displays it. This complements the echo
 * server from Exercise 1.
 * 
 * REQUIREMENTS:
 * 1. Create a TCP socket
 * 2. Connect to server at localhost:8080
 * 3. Send a message to the server
 * 4. Receive and display the server's response
 * 5. Close the connection gracefully
 * 
 * EXPECTED OUTPUT:
 * Client: Connecting to server...
 * Client: Connected!
 * Client: Sending: Hello from client!
 * Client: Received: Hello from client!
 * Client: Connection closed
 * 
 * HINTS:
 * - Use connect() to establish connection to server
 * - inet_pton() converts IP address string to binary form
 * - Check return values of send() and recv()
 * - recv() may return 0 when server closes connection
 * 
 * BONUS CHALLENGE:
 * Make the client interactive - read messages from user input
 * and send them to the server in a loop until user types "quit"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    char message[] = "Hello from client!";
    char buffer[BUFFER_SIZE] = {0};
    
    // TODO: Create a TCP socket
    
    // TODO: Setup server address structure
    // - Set address family
    // - Set port (remember network byte order)
    // - Convert IP address string to binary
    
    printf("Client: Connecting to server...\n");
    
    // TODO: Connect to the server
    
    printf("Client: Connected!\n");
    
    // TODO: Send message to server
    printf("Client: Sending: %s\n", message);
    
    // TODO: Receive response from server
    
    // TODO: Display received message
    
    // TODO: Close the socket
    
    printf("Client: Connection closed\n");
    
    return 0;
}