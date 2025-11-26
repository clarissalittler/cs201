// echo_server_threaded.c
// Multi-client echo server using pthreads.
// Compile: gcc -o echo_server_threaded echo_server_threaded.c -pthread
// Usage: ./echo_server_threaded port

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void *handle_client(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Multi-client echo server %d listening on port %d...\n", getpid(), port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Create a thread to handle this client
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(client_fd_ptr);
            continue;
        }
        pthread_detach(thread);
    }

    return 0;
}

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[1024];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        send(client_fd, buffer, bytes_received, 0);
    }

    printf("Client disconnected.\n");
    close(client_fd);
    return NULL;
}
