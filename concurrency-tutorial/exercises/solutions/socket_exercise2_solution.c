/*
 * Socket Exercise 2 Solution: TCP Client
 * 
 * This solution shows how to create a TCP client that:
 * - Connects to a server
 * - Sends a message
 * - Receives a response
 * - Handles errors appropriately
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
    int client_socket;
    struct sockaddr_in server_addr;
    char message[] = "Hello from client!";
    char buffer[BUFFER_SIZE] = {0};
    int bytes_received;
    
    // Create TCP socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Setup server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    // Convert IP address from string to binary
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Client: Connecting to server...\n");
    
    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, 
                sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Client: Connected!\n");
    
    // Send message to server
    printf("Client: Sending: %s\n", message);
    if (send(client_socket, message, strlen(message), 0) < 0) {
        perror("Send failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    
    // Receive response from server
    bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
    } else if (bytes_received == 0) {
        printf("Client: Server closed connection\n");
    } else {
        buffer[bytes_received] = '\0';
        printf("Client: Received: %s\n", buffer);
    }
    
    // Close socket
    close(client_socket);
    printf("Client: Connection closed\n");
    
    return 0;
}

/*
 * BONUS: Interactive client implementation
 * Uncomment the function below and replace the send/receive section
 * in main() with a call to interactive_client(client_socket)
 */

/*
void interactive_client(int socket) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    printf("Client: Enter messages (type 'quit' to exit)\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        
        // Remove newline
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // Check for quit command
        if (strcmp(buffer, "quit") == 0) {
            printf("Client: Quitting...\n");
            break;
        }
        
        // Send message
        if (send(socket, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            break;
        }
        
        // Receive response
        int bytes = recv(socket, response, BUFFER_SIZE - 1, 0);
        if (bytes < 0) {
            perror("Receive failed");
            break;
        } else if (bytes == 0) {
            printf("Client: Server closed connection\n");
            break;
        }
        
        response[bytes] = '\0';
        printf("Server: %s\n", response);
    }
}
*/