// webserver_v2.c
// Web server that serves static files.
// Compile: gcc -o webserver_v2 webserver_v2.c
// Usage: ./webserver_v2 port webroot
// Example: ./webserver_v2 8080 ./public

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUFFER_SIZE 8192
#define MAX_PATH 512

char *webroot;

void handle_client(int client_fd);
void send_response(int client_fd, int status, char *status_text,
                   char *content_type, char *body, int body_len);
void send_file(int client_fd, char *path);
void send_error(int client_fd, int status, char *status_text);
char *get_content_type(char *path);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s port webroot\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    webroot = argv[2];

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

    printf("Web server running on http://localhost:%d\n", port);
    printf("Serving files from: %s\n", webroot);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        handle_client(client_fd);
        close(client_fd);
    }

    return 0;
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return;
    buffer[bytes] = '\0';

    // Parse the request line: GET /path HTTP/1.1
    char method[16], path[MAX_PATH], version[16];
    if (sscanf(buffer, "%15s %511s %15s", method, path, version) != 3) {
        send_error(client_fd, 400, "Bad Request");
        return;
    }

    printf("%s %s %s\n", method, path, version);

    // Only handle GET requests
    if (strcmp(method, "GET") != 0) {
        send_error(client_fd, 405, "Method Not Allowed");
        return;
    }

    // Security: reject paths with ".." to prevent directory traversal
    if (strstr(path, "..") != NULL) {
        send_error(client_fd, 403, "Forbidden");
        return;
    }

    // Build the full file path
    char full_path[MAX_PATH];
    if (strcmp(path, "/") == 0) {
        snprintf(full_path, sizeof(full_path), "%s/index.html", webroot);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", webroot, path);
    }

    send_file(client_fd, full_path);
}

void send_file(int client_fd, char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        send_error(client_fd, 404, "Not Found");
        return;
    }

    // Get file size
    struct stat st;
    fstat(fd, &st);
    int file_size = st.st_size;

    // Read file contents
    char *body = malloc(file_size);
    if (body == NULL) {
        close(fd);
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }
    read(fd, body, file_size);
    close(fd);

    // Send response
    char *content_type = get_content_type(path);
    send_response(client_fd, 200, "OK", content_type, body, file_size);

    free(body);
}

void send_response(int client_fd, int status, char *status_text,
                   char *content_type, char *body, int body_len) {
    char header[BUFFER_SIZE];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, status_text, content_type, body_len);

    send(client_fd, header, header_len, 0);
    send(client_fd, body, body_len, 0);
}

void send_error(int client_fd, int status, char *status_text) {
    char body[256];
    int body_len = snprintf(body, sizeof(body),
        "<html><body><h1>%d %s</h1></body></html>",
        status, status_text);
    send_response(client_fd, status, status_text, "text/html", body, body_len);
}

char *get_content_type(char *path) {
    char *ext = strrchr(path, '.');
    if (ext == NULL) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
        return "text/html";
    if (strcmp(ext, ".css") == 0)
        return "text/css";
    if (strcmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcmp(ext, ".png") == 0)
        return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(ext, ".gif") == 0)
        return "image/gif";
    if (strcmp(ext, ".txt") == 0)
        return "text/plain";

    return "application/octet-stream";
}
