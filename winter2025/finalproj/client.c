#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(SERVER_PORT);

    // Convert localhost (127.0.0.1) or use INADDR_ANY if on same machine
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Type your responses below.\n");

    // Communication loop
    while (1) {
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Read prompt from server
        int valread = read(sockfd, buffer, BUFFER_SIZE - 1);
        if (valread <= 0) {
            // Could be a disconnect or error
            printf("Server disconnected or error occurred.\n");
            break;
        }

        // Display prompt to user
        buffer[valread] = '\0';
        printf("%s", buffer);

        // Get user input
        char userInput[BUFFER_SIZE];
        if (!fgets(userInput, BUFFER_SIZE, stdin)) {
            // If input fails, close
            printf("Error reading input or EOF.\n");
            break;
        }

        // Send user input to server
        int toSend = strlen(userInput);
        if (write(sockfd, userInput, toSend) < 0) {
            perror("write");
            break;
        }
    }

    // Cleanup
    close(sockfd);
    return 0;
}
