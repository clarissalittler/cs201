/*
 * Exercise 4: Concurrent Server with Processes
 * Difficulty: 🟡 Intermediate
 * 
 * OBJECTIVE:
 * Create a concurrent server that can handle multiple clients
 * simultaneously using fork(). Each client gets its own process.
 * 
 * REQUIREMENTS:
 * 1. Create a TCP server that listens on port 8080
 * 2. For each accepted connection:
 *    - Fork a child process to handle the client
 *    - Parent continues accepting new connections
 *    - Child handles all communication with its client
 * 3. Implement a simple protocol:
 *    - Client sends name as first message
 *    - Server responds with personalized greeting
 *    - Server prefixes all subsequent echoes with client's name
 * 4. Clean up zombie processes
 * 5. Handle up to 10 simultaneous clients
 * 
 * PROTOCOL EXAMPLE:
 * Client: Alice
 * Server: Welcome, Alice!
 * Client: Hello
 * Server: [Alice]: Hello
 * Client: quit
 * Server: Goodbye, Alice!
 * 
 * HINTS:
 * - Parent should close client socket after fork
 * - Child should close server socket
 * - Use signal(SIGCHLD, SIG_IGN) to avoid zombies
 * - Or use waitpid() with WNOHANG in a signal handler
 * 
 * COMMON PITFALLS:
 * - File descriptor leaks (not closing in correct process)
 * - Zombie processes accumulating
 * - Parent process handling client (should be child)
 * 
 * TESTING:
 * Use multiple terminal windows with nc or telnet to test
 * concurrent connections
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_NAME_LEN 50
#define LISTEN_BACKLOG 10

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char client_name[MAX_NAME_LEN];
    char response[BUFFER_SIZE];
    
    // TODO: Read client's name (first message)
    
    // TODO: Send welcome message
    
    // TODO: Echo loop with name prefix
    while (1) {
        // TODO: Receive message
        
        // TODO: Check for "quit" command
        
        // TODO: Create response with name prefix
        
        // TODO: Send response
    }
    
    // TODO: Send goodbye message
    
    // TODO: Close client socket
}

int main() {
    // TODO: Set up signal handler for SIGCHLD to prevent zombies
    
    // TODO: Create server socket
    
    // TODO: Set SO_REUSEADDR
    
    // TODO: Bind and listen
    
    printf("Server: Concurrent server listening on port %d\n", PORT);
    
    // TODO: Main accept loop
    while (1) {
        // TODO: Accept client
        
        // TODO: Fork to handle client
        
        // TODO: In child process:
        //       - Close server socket
        //       - Handle client
        //       - Exit child process
        
        // TODO: In parent process:
        //       - Close client socket
        //       - Continue loop
    }
    
    return 0;
}