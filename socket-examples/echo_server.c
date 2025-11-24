// echo_server.c
// A simple TCP echo server.
// Compile: gcc -o echo_server echo_server.c
// Usage: ./echo_server port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    // Step 1: Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    // Allow address reuse (helpful during development)
    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Step 2: Bind to address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    // Step 3: Listen for connections
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("Echo server listening on port %d...\n", port);

    // Step 4: Accept and handle connections
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;  // Keep trying
        }

        // Convert client IP to string for logging
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Step 5: Echo loop
        char buffer[1024];
        ssize_t bytes_received;

        while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            // Echo back what we received
            ssize_t bytes_sent = send(client_fd, buffer, bytes_received, 0);
            if (bytes_sent == -1) {
                perror("send");
                break;
            }
        }

        if (bytes_received == -1) {
            perror("recv");
        }

        printf("Client %s disconnected.\n", client_ip);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
