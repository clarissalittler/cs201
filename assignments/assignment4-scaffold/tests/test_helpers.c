/* Instructor-provided helper tests: no student handler or solution here. */
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

static const unsigned char *input;
static size_t inputLength, inputAt, readChunk;
static unsigned char output[8192];
static size_t outputAt, writeChunk;
static int readInterrupt, writeInterrupt, readFail, writeFail, writeZero;

static ssize_t fakeRead(int fd, void *buf, size_t length) {
    (void)fd;
    if (readInterrupt) { readInterrupt = 0; errno = EINTR; return -1; }
    if (readFail) { errno = EIO; return -1; }
    size_t n = inputLength - inputAt;
    if (n > length) n = length;
    if (n > readChunk) n = readChunk;
    memcpy(buf, input + inputAt, n);
    inputAt += n;
    return (ssize_t)n;
}
static ssize_t fakeWrite(int fd, const void *buf, size_t length) {
    (void)fd;
    if (writeInterrupt) { writeInterrupt = 0; errno = EINTR; return -1; }
    if (writeFail) { errno = EPIPE; return -1; }
    if (writeZero) return 0;
    size_t n = length < writeChunk ? length : writeChunk;
    assert(outputAt + n <= sizeof(output));
    memcpy(output + outputAt, buf, n);
    outputAt += n;
    return (ssize_t)n;
}
#define HTTP_READ fakeRead
#define HTTP_WRITE fakeWrite
#include "../http_support.c"

static void reset(const void *bytes, size_t n) {
    input = bytes; inputLength = n; inputAt = 0; readChunk = 8192;
    outputAt = 0; writeChunk = 8192;
    readInterrupt = writeInterrupt = readFail = writeFail = writeZero = 0;
}
static int parse(const char *text) {
    Request request;
    return parseRequest(text, &request);
}
int main(void) {
    char header[HEADER_LIMIT + 1];
    const char request[] = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    for (size_t n = 1; n <= sizeof(request); ++n) {
        reset(request, strlen(request)); readChunk = n; readInterrupt = 1;
        assert(readHeader(0, header) == (ssize_t)strlen(request));
        assert(strcmp(header, request) == 0);
    }
    reset(request, 8); assert(readHeader(0, header) == 0);
    reset(request, strlen(request)); readFail = 1; assert(readHeader(0, header) == -1);
    const char nul[] = "GET /\0 HTTP/1.0\r\n\r\n";
    reset(nul, sizeof(nul)-1); assert(readHeader(0, header) == -3);
    const char extra[] = "GET / HTTP/1.0\r\n\r\nignored\0body";
    reset(extra, sizeof(extra)-1); assert(readHeader(0, header) == 18);
    char boundary[HEADER_LIMIT + 2];
    memset(boundary, 'a', sizeof(boundary));
    memcpy(boundary + HEADER_LIMIT - 4, "\r\n\r\n", 4);
    reset(boundary, HEADER_LIMIT); readChunk = 7;
    assert(readHeader(0, header) == HEADER_LIMIT && header[HEADER_LIMIT] == '\0');
    memset(boundary, 'a', sizeof(boundary));
    memcpy(boundary + HEADER_LIMIT - 3, "\r\n\r\n", 4);
    reset(boundary, HEADER_LIMIT + 1); assert(readHeader(0, header) == -2);

    const unsigned char body[] = {0,1,2,0,255,10,99};
    reset(body, sizeof(body)); writeChunk = 2; writeInterrupt = 1;
    assert(sendAll(0, body, sizeof(body)) == 0);
    assert(outputAt == sizeof(body) && !memcmp(output, body, sizeof(body)));
    reset(body, sizeof(body)); writeFail = 1;
    assert(sendAll(0, body, sizeof(body)) == -1 && errno == EPIPE);
    reset(body, sizeof(body)); writeZero = 1;
    assert(sendAll(0, body, sizeof(body)) == -1 && errno == EIO);
    reset(body, sizeof(body)); assert(sendAll(0, body, 0) == 0 && outputAt == 0);
    reset(body, sizeof(body)); writeChunk = 3; writeInterrupt = 1;
    assert(sendText(0, 404, "Not Found", "missing") == 0);
    output[outputAt] = '\0';
    assert(strstr((char *)output, "Content-Length: 7\r\n") != NULL);
    assert(strstr((char *)output, "\r\n\r\nmissing") != NULL);
    char tooLong[600]; memset(tooLong, 'x', sizeof(tooLong)-1); tooLong[599] = '\0';
    reset(body, sizeof(body));
    assert(sendStatus(0, 200, tooLong, "text/plain", 0) == -1 && outputAt == 0);

    assert(parse(request) == 0);
    assert(parse("GET / HTTP/1.0\r\n\r\n") == 0);
    assert(parse("GET /a_b/file-1.txt HTTP/1.1\r\nContent-Length: 0\r\n\r\n") == 0);
    const char *bad[] = {"/../secret", "/a/../secret", "/./file", "/a//b",
        "/a/", "/%2e%2e/a", "/a?b", "/a#b", "/a\\b", "http://example/a"};
    for (size_t i = 0; i < sizeof(bad)/sizeof(bad[0]); ++i) {
        snprintf(header, sizeof(header), "GET %s HTTP/1.0\r\n\r\n", bad[i]);
        assert(parse(header) == 400);
    }
    assert(parse("POST / HTTP/1.0\r\n\r\n") == 405);
    assert(parse("GET / HTTP/2.0\r\n\r\n") == 505);
    assert(parse("GET / HTTP/1.0\n\n") == 400);
    assert(parse("GET  / HTTP/1.0\r\n\r\n") == 400);
    assert(parse("GET / HTTP/1.0\r\nMalformed\r\n\r\n") == 400);
    assert(parse("GET / HTTP/1.0\r\nX: a\nb\r\n\r\n") == 400);
    assert(parse("GET / HTTP/1.0\r\nContent-Length: 1\r\n\r\n") == 400);
    assert(parse("GET / HTTP/1.0\r\nContent-Length: 0\r\nContent-Length: 0\r\n\r\n") == 400);
    assert(parse("GET / HTTP/1.0\r\nTransfer-Encoding: chunked\r\n\r\n") == 400);
    char path[PATH_LIMIT+2]; memset(path, 'a', sizeof(path)); path[0] = '/';
    path[PATH_LIMIT] = '\0';
    snprintf(header, sizeof(header), "GET %s HTTP/1.0\r\n\r\n", path);
    assert(parse(header) == 0);
    path[PATH_LIMIT] = 'a'; path[PATH_LIMIT+1] = '\0';
    snprintf(header, sizeof(header), "GET %s HTTP/1.0\r\n\r\n", path);
    assert(parse(header) == 400);
    unsigned short port;
    assert(parsePort("8080", &port) == 0 && port == 8080);
    assert(parsePort("65535", &port) == 0 && port == 65535);
    const char *badPorts[] = {"", "0", "65536", "-1", "80x", "999999999999", " 80"};
    for (size_t i = 0; i < sizeof(badPorts)/sizeof(badPorts[0]); ++i)
        assert(parsePort(badPorts[i], &port) == -1);
    assert(!strcmp(mimeFor("a.png"), "image/png"));
    assert(!strcmp(mimeFor("a.bin"), "application/octet-stream"));
    puts("Helper tests passed: fragmented/short/interrupted I/O, limits, parsing, paths, ports.");
    return 0;
}
