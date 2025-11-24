// webserver_threaded.c
// Multi-client web server using pthreads.
// Compile: gcc -o webserver_threaded webserver_threaded.c -pthread
// Usage: ./webserver_threaded port webroot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFFER_SIZE 8192
#define MAX_PATH 512

char *webroot;

void *client_thread(void *arg);
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

    printf("Web server (threaded) running on http://localhost:%d\n", port);
    printf("Serving files from: %s\n", webroot);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        int *client_fd_ptr = malloc(sizeof(int));
        if (client_fd_ptr == NULL) {
            perror("malloc");
            close(client_fd);
            continue;
        }
        *client_fd_ptr = client_fd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, client_thread, client_fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(client_fd_ptr);
            continue;
        }

        pthread_detach(thread);
    }

    return 0;
}

void *client_thread(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    handle_client(client_fd);
    close(client_fd);

    return NULL;
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return;
    buffer[bytes] = '\0';

    char method[16], path[MAX_PATH], version[16];
    if (sscanf(buffer, "%15s %511s %15s", method, path, version) != 3) {
        send_error(client_fd, 400, "Bad Request");
        return;
    }

    printf("[Thread %lu] %s %s %s\n", (unsigned long)pthread_self(), method, path, version);

    if (strcmp(method, "GET") != 0) {
        send_error(client_fd, 405, "Method Not Allowed");
        return;
    }

    if (strstr(path, "..") != NULL) {
        send_error(client_fd, 403, "Forbidden");
        return;
    }

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

    struct stat st;
    fstat(fd, &st);
    int file_size = st.st_size;

    char *body = malloc(file_size);
    if (body == NULL) {
        close(fd);
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }
    read(fd, body, file_size);
    close(fd);

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
