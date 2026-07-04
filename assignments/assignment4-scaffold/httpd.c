/* httpd.c -- Tiny HTTP/1.0 server scaffold
 *
 * Build:  make
 * Run:    ./httpd
 * Test:   curl http://localhost:8080/
 *         or open http://localhost:8080/ in a browser
 *
 * The accept loop and thread spawning are written for you.
 * Your job is to fill in handleRequest() below.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>

#define PORT     8080
#define BUF_SIZE 4096
#define DOCROOT  "./www"

/* ============ Helpers (provided) ============ */

/* Send an HTTP response header to the client.
 * Call once per response, before sending the body.
 * Example:  sendStatus(sock, 200, "OK", "text/html", 137);
 */
static void sendStatus(int sock, int code, const char* reason,
                       const char* contentType, long contentLength) {
    char header[512];
    int n = snprintf(header, sizeof(header),
        "HTTP/1.0 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        code, reason, contentType, contentLength);
    write(sock, header, n);
}

/* Look up a Content-Type based on a file path's extension.
 * Extend this if you want to support more types.
 */
static const char* mimeFor(const char* path) {
    const char* dot = strrchr(path, '.');
    if (!dot) return "application/octet-stream";
    if (!strcmp(dot, ".html") || !strcmp(dot, ".htm")) return "text/html";
    if (!strcmp(dot, ".txt"))  return "text/plain";
    if (!strcmp(dot, ".css"))  return "text/css";
    if (!strcmp(dot, ".js"))   return "application/javascript";
    if (!strcmp(dot, ".png"))  return "image/png";
    if (!strcmp(dot, ".jpg") || !strcmp(dot, ".jpeg")) return "image/jpeg";
    return "application/octet-stream";
}

/* ============ The part you write ============ */

/* Handle one request on the given socket, then return.
 * The caller will close the socket for you.
 *
 * Steps:
 *   1. Read up to BUF_SIZE-1 bytes from sock into a buffer. Null-terminate.
 *   2. Parse the request line. It looks like:  "GET /path HTTP/1.0"
 *      Hint: sscanf with a "%s %s" format reads method and path.
 *   3. Build the on-disk path:
 *        - If the URL path is "/", use DOCROOT "/index.html".
 *        - Otherwise, concatenate DOCROOT and the URL path
 *          (e.g. "/about.html" becomes "./www/about.html").
 *   4. Try to open the file with fopen(path, "rb")  -- "rb" = binary read.
 *        - On failure: call sendStatus(sock, 404, "Not Found", "text/html", bodyLen),
 *          then write a short HTML body to sock.
 *        - On success: get the file size (stat() or fseek+ftell),
 *          call sendStatus(sock, 200, "OK", mimeFor(path), size),
 *          then loop reading the file with fread() and writing to sock
 *          until fread returns 0.
 *
 * Remember: free anything you malloc, fclose anything you fopen.
 */
static void handleRequest(int sock) {
    (void)sock;
    /* Fill this out!!
     * (Until you use the helpers above, the compiler will warn that
     * sendStatus and mimeFor are unused. That's expected -- the warnings
     * go away once your implementation calls them.) */
}

static void* clientThread(void* arg) {
    int sock = *(int*)arg;
    free(arg);
    handleRequest(sock);
    close(sock);
    return NULL;
}

int main(void) {
    /* If a client hangs up mid-response (browsers do this all the time),
     * write() to the dead socket raises SIGPIPE, which kills the whole
     * process by default. Ignoring it makes write() return -1 instead,
     * so one rude client can't take down the server. */
    signal(SIGPIPE, SIG_IGN);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(srv, 16) < 0) {
        perror("listen"); return 1;
    }
    printf("httpd listening on http://localhost:%d/\n", PORT);

    while(1) {
        int cli = accept(srv, NULL, NULL);
        if (cli < 0) { perror("accept"); continue; }

        int* arg = malloc(sizeof(int));
        if (!arg) { close(cli); continue; }
        *arg = cli;

        pthread_t t;
        if (pthread_create(&t, NULL, clientThread, arg) != 0) {
            perror("pthread_create");
            free(arg);
            close(cli);
            continue;
        }
        pthread_detach(t);
    }
    return 0;
}
