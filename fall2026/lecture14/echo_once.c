#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h> /* htons, htonl, ntohs */
#include <errno.h>     /* EINTR, EIO */
#include <stdio.h>     /* printf, perror */
#include <stdlib.h>    /* strtol */
#include <sys/socket.h> /* socket, bind, listen, accept, send, recv */
#include <unistd.h>    /* close */

static int send_all(int fd, const char *buffer, size_t count) {
    while (count > 0) {
        /* Linux: a disconnected peer is an error, not a SIGPIPE exit. */
        ssize_t sent = send(fd, buffer, count, MSG_NOSIGNAL);
        if (sent == -1 && errno == EINTR) continue;
        if (sent <= 0) {
            if (sent == 0) errno = EIO;
            return -1;
        }
        buffer += sent;
        count -= (size_t)sent;
    }
    return 0;
}

int main(int argc, char **argv) {
    char *end;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s PORT (0 chooses a free port)\n", argv[0]);
        return 1;
    }
    errno = 0;
    long port = strtol(argv[1], &end, 10);
    if (errno || end == argv[1] || *end || port < 0 || port > 65535) {
        fputs("PORT must be 0..65535\n", stderr);
        return 1;
    }
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) { perror("socket"); return 1; }
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons((unsigned short)port);
    if (bind(server, (struct sockaddr *)&address, sizeof address) == -1 ||
        listen(server, 1) == -1) {
        perror("bind/listen");
        close(server);
        return 1;
    }
    socklen_t length = sizeof address;
    if (getsockname(server, (struct sockaddr *)&address, &length) == -1) {
        perror("getsockname"); close(server); return 1;
    }
    printf("Listening on 127.0.0.1:%u\n", (unsigned)ntohs(address.sin_port));
    fflush(stdout);
    int client;
    do { client = accept(server, NULL, NULL); } while (client == -1 && errno == EINTR);
    if (client == -1) { perror("accept"); close(server); return 1; }
    close(server); /* this version serves exactly one connection */
    char buffer[1024];
    int result = 0;
    for (;;) {
        ssize_t count = recv(client, buffer, sizeof buffer, 0);
        if (count == -1 && errno == EINTR) continue;
        if (count == 0) break;
        if (count == -1 || send_all(client, buffer, (size_t)count) == -1) {
            perror("echo");
            result = 1;
            break;
        }
    }
    if (close(client) == -1) { perror("close"); result = 1; }
    return result;
}
