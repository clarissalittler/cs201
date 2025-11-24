// chat_client.c
// Chat client with separate threads for sending and receiving.
// Compile: gcc -o chat_client chat_client.c -pthread
// Usage: ./chat_client hostname port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int sockfd;
volatile int running = 1;

void *receive_thread(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    char *hostname = argv[1];
    char *port = argv[2];

    // Resolve hostname
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Create socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    // Connect
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    freeaddrinfo(res);
    printf("Connected to %s:%s\n", hostname, port);
    printf("Type messages and press Enter. Ctrl+C to quit.\n\n");

    // Start receive thread
    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receive_thread, NULL) != 0) {
        perror("pthread_create");
        close(sockfd);
        exit(1);
    }

    // Main thread handles sending
    char buffer[BUFFER_SIZE];
    while (running && fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            break;
        }
    }

    running = 0;
    close(sockfd);
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);

    printf("\nDisconnected.\n");
    return 0;
}

void *receive_thread(void *arg) {
    (void)arg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes;

    while (running && (bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }

    if (bytes == 0 && running) {
        printf("\nServer closed connection.\n");
        running = 0;
    }

    return NULL;
}
