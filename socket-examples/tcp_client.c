// tcp_client.c
// A simple TCP client that sends a message and prints the response.
// Compile: gcc -o tcp_client tcp_client.c
// Usage: ./tcp_client hostname port message

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s hostname port message\n", argv[0]);
        exit(1);
    }

    char *hostname = argv[1];
    char *port = argv[2];
    char *message = argv[3];

    // Step 1: Resolve the hostname
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Step 2: Create a socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(res);
        exit(1);
    }

    // Step 3: Connect to the server
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);  // No longer needed

    // Step 4: Send the message
    ssize_t sent = send(sockfd, message, strlen(message), 0);
    if (sent == -1) {
        perror("send");
        close(sockfd);
        exit(1);
    }
    printf("Sent %zd bytes: %s\n", sent, message);

    // Step 5: Receive the response
    char buffer[4096];
    ssize_t received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received == -1) {
        perror("recv");
    } else if (received == 0) {
        printf("Server closed connection.\n");
    } else {
        buffer[received] = '\0';
        printf("Received %zd bytes:\n%s\n", received, buffer);
    }

    // Step 6: Clean up
    close(sockfd);
    return 0;
}
