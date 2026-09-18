/* Tiny HTTP server scaffold. Build: make. Run: ./httpd [port]
 * Run from this directory; files are served from ./www.
 * Fill in handleRequest. Request validation and I/O helpers are supplied
 * in http_support.c. See function-guide.md for their contracts. */
#define _POSIX_C_SOURCE 200809L
#include "http_support.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

/* ============ The part you write ============ */
/* request->path is validated, but '/' still needs mapping to '/index.html'.
 * The request lives until this function returns.
 * 1. Start by sending a fixed response using sendText.
 * 2. Build DOCROOT + the path. Map '/' to '/index.html'. Check snprintf.
 * 3. Open the file in binary mode. Use fstat(fileno(file), &st) and
 *    S_ISREG(st.st_mode) to check the opened object is a regular file.
 *    If it cannot be served, close any opened file and send a 404 body.
 * 4. Send headers with sendStatus and the file size, then use fread and
 *    sendAll to transfer chunks. Stop on errors and always fclose the file.
 * Use a controlled www directory without symlinks. Don't change files
 * while serving them. The worker closes the socket; you do NOT. */
static void handleRequest(int sock, const Request *request) {
    (void)request;
    /* TODO: replace this placeholder with your handler. */
    (void)sendText(sock, 501, "Not Implemented",
                   "<p>The request handler is not implemented yet.</p>\n");
}

/* ============ Connection management (provided) ============ */
static void *clientThread(void *arg) {
    int sock = *(int *)arg;
    free(arg);
    char header[HEADER_LIMIT + 1];
    Request request;
    ssize_t length = readHeader(sock, header);
    if (length > 0) {
        int status = parseRequest(header, &request);
        if (status == 0) {
            handleRequest(sock, &request);
        } else if (status == 405) {
            (void)sendText(sock, 405, "Method Not Allowed", "<p>Use GET.</p>\n");
        } else if (status == 505) {
            (void)sendText(sock, 505, "HTTP Version Not Supported",
                           "<p>Use HTTP/1.0 or HTTP/1.1.</p>\n");
        } else {
            (void)sendText(sock, 400, "Bad Request", "<p>Unsupported request.</p>\n");
        }
    } else if (length == -2) {
        (void)sendText(sock, 431, "Request Header Fields Too Large",
                       "<p>Headers may use at most 4096 bytes.</p>\n");
    } else if (length == -3 || length == 0) {
        (void)sendText(sock, 400, "Bad Request", "<p>Incomplete or invalid request.</p>\n");
    }
    close(sock);
    return NULL;
}

int main(int argc, char **argv) {
    unsigned short port = 8080;
    if (argc > 2 || (argc == 2 && parsePort(argv[1], &port) < 0)) {
        fprintf(stderr, "Usage: %s [port: 1-65535]\n", argv[0]); return 1;
    }
    /* A disconnected client must not terminate the entire server. */
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGPIPE, &action, NULL) < 0) { perror("sigaction"); return 1; }
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return 1; }
    int opt = 1;
    if (setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt"); close(srv); return 1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);
    if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind (try another port)"); close(srv); return 1;
    }
    if (listen(srv, 32) < 0) { perror("listen"); close(srv); return 1; }
    pthread_attr_t attributes;
    int error = pthread_attr_init(&attributes);
    if (error) { fprintf(stderr, "pthread_attr_init: %s\n", strerror(error)); close(srv); return 1; }
    error = pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    if (error) {
        fprintf(stderr, "pthread_attr_setdetachstate: %s\n", strerror(error));
        pthread_attr_destroy(&attributes); close(srv); return 1;
    }
    printf("httpd listening on http://localhost:%hu/ (Ctrl-C to stop)\n", port);
    fflush(stdout);
    for (;;) {
        int cli = accept(srv, NULL, NULL);
        if (cli < 0) {
            if (errno == EINTR) continue;
            perror("accept"); break;
        }
        int *arg = malloc(sizeof(*arg));
        if (!arg) { perror("malloc"); close(cli); continue; }
        *arg = cli;
        pthread_t thread;
        error = pthread_create(&thread, &attributes, clientThread, arg);
        if (error) {
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            free(arg); close(cli);
        }
    }
    pthread_attr_destroy(&attributes);
    close(srv);
    return 1;
}
