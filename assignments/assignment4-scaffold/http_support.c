#define _POSIX_C_SOURCE 200809L
#include "http_support.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

/* Tests replace these operations to force short I/O and EINTR. */
#ifndef HTTP_READ
#define HTTP_READ read
#endif
#ifndef HTTP_WRITE
#define HTTP_WRITE write
#endif

int sendAll(int sock, const void *data, size_t length) {
    const unsigned char *bytes = data;
    size_t sent = 0;
    while (sent < length) {
        ssize_t n = HTTP_WRITE(sock, bytes + sent, length - sent);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) { errno = EIO; return -1; }
        sent += (size_t)n;
    }
    return 0;
}

ssize_t readHeader(int sock, char header[HEADER_LIMIT + 1]) {
    size_t used = 0;
    header[0] = '\0';
    while (used < HEADER_LIMIT) {
        ssize_t n = HTTP_READ(sock, header + used, HEADER_LIMIT - used);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) return 0;
        size_t previous = used;
        used += (size_t)n;
        header[used] = '\0';
        /* Scan bytes: embedded NUL is malformed, not end-of-header.
         * Ignore bytes after the first header; we close after one response. */
        for (size_t i = previous; i < used; ++i) {
            if (header[i] == '\0') return -3;
            if (i >= 3 && memcmp(header + i - 3, "\r\n\r\n", 4) == 0) {
                header[i + 1] = '\0';
                return (ssize_t)(i + 1);
            }
        }
    }
    return -2;
}

static int simplePath(const char *path) {
    if (path[0] != '/') return 0;
    if (path[1] == '\0') return 1;
    const char *segment = path + 1;
    for (const char *p = segment; ; ++p) {
        unsigned char c = (unsigned char)*p;
        if (c == '/' || c == '\0') {
            size_t n = (size_t)(p - segment);
            if (n == 0 || (n == 1 && segment[0] == '.') ||
                (n == 2 && segment[0] == '.' && segment[1] == '.')) return 0;
            if (c == '\0') return 1;
            segment = p + 1;
        } else if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                     (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.')) {
            return 0;
        }
    }
}

static int headerNameChar(unsigned char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || strchr("!#$%&'*+-.^_`|~", c) != NULL;
}

int parseRequest(const char *header, Request *request) {
    const char *end = strstr(header, "\r\n");
    if (!end) return 400;
    size_t lineLength = (size_t)(end - header);
    char line[HEADER_LIMIT + 1];
    if (lineLength >= sizeof(line)) return 400;
    memcpy(line, header, lineLength);
    line[lineLength] = '\0';
    /* Exactly METHOD SP TARGET SP VERSION, with bounded storage. */
    char *first = strchr(line, ' ');
    if (!first || first == line) return 400;
    *first++ = '\0';
    char *second = strchr(first, ' ');
    if (!second || second == first) return 400;
    *second++ = '\0';
    if (strchr(second, ' ') || strchr(second, '\t')) return 400;
    if (strcmp(line, "GET") != 0) return 405;
    if (strcmp(second, "HTTP/1.0") && strcmp(second, "HTTP/1.1")) return 505;
    if (strlen(first) > PATH_LIMIT || !simplePath(first)) return 400;
    int sawLength = 0;
    const char *p = end + 2;
    while (*p) {
        const char *next = strstr(p, "\r\n");
        if (!next) return 400;
        if (next == p) {
            if (next[2] != '\0') return 400;
            strcpy(request->path, first); /* bounded above */
            return 0;
        }
        const char *colon = memchr(p, ':', (size_t)(next - p));
        if (!colon || colon == p) return 400;
        for (const char *q = p; q < colon; ++q)
            if (!headerNameChar((unsigned char)*q)) return 400;
        for (const char *q = colon + 1; q < next; ++q) {
            unsigned char c = (unsigned char)*q;
            if ((c < 32 && c != '\t') || c == 127) return 400;
        }
        size_t nameLength = (size_t)(colon - p);
        if (nameLength == 17 && !strncasecmp(p, "Transfer-Encoding", 17)) return 400;
        if (nameLength == 14 && !strncasecmp(p, "Content-Length", 14)) {
            if (sawLength++) return 400;
            const char *value = colon + 1;
            while (value < next && (*value == ' ' || *value == '\t')) ++value;
            if (value == next || *value++ != '0') return 400;
            while (value < next && (*value == ' ' || *value == '\t')) ++value;
            if (value != next) return 400; /* no request body in this exercise */
        }
        p = next + 2;
    }
    return 400;
}

int sendStatus(int sock, int code, const char *reason,
               const char *contentType, uintmax_t contentLength) {
    char header[512];
    int n = snprintf(header, sizeof(header),
                     "HTTP/1.0 %d %s\r\n"
                     "Content-Type: %s\r\n"
                     "Content-Length: %" PRIuMAX "\r\n"
                     "Connection: close\r\n\r\n",
                     code, reason, contentType, contentLength);
    if (n < 0 || (size_t)n >= sizeof(header)) { errno = EOVERFLOW; return -1; }
    return sendAll(sock, header, (size_t)n);
}

int sendText(int sock, int code, const char *reason, const char *body) {
    size_t n = strlen(body);
    if (sendStatus(sock, code, reason, "text/html", (uintmax_t)n) < 0) return -1;
    return sendAll(sock, body, n);
}

const char *mimeFor(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return "application/octet-stream";
    if (!strcmp(dot, ".html") || !strcmp(dot, ".htm")) return "text/html";
    if (!strcmp(dot, ".txt")) return "text/plain";
    if (!strcmp(dot, ".css")) return "text/css";
    if (!strcmp(dot, ".png")) return "image/png";
    if (!strcmp(dot, ".jpg") || !strcmp(dot, ".jpeg")) return "image/jpeg";
    return "application/octet-stream";
}

int parsePort(const char *text, unsigned short *port) {
    unsigned value = 0;
    if (!*text) return -1;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        if (*p < '0' || *p > '9') return -1;
        value = value * 10U + (unsigned)(*p - '0');
        if (value > 65535U) return -1;
    }
    if (value == 0) return -1;
    *port = (unsigned short)value;
    return 0;
}
