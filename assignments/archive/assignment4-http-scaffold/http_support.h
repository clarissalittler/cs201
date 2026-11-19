#ifndef HTTP_SUPPORT_H
#define HTTP_SUPPORT_H
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/* Limit includes the final CRLF CRLF, but excludes the C string terminator. */
#define HEADER_LIMIT 4096
#define PATH_LIMIT 255
#define TRANSFER_SIZE 4096
#define DOCROOT "./www"
typedef struct { char path[PATH_LIMIT + 1]; } Request;

/* Positive length = complete header; 0 = early EOF; -1 = I/O error;
 * -2 = too large; -3 = embedded NUL. Supply HEADER_LIMIT+1 bytes. */
ssize_t readHeader(int sock, char header[HEADER_LIMIT + 1]);
/* 0 = accepted; otherwise 400/405/505. Input comes from readHeader. */
int parseRequest(const char *header, Request *request);
/* 0 = success; -1 = error. Handles short writes and EINTR. */
int sendAll(int sock, const void *data, size_t length);
int sendStatus(int sock, int code, const char *reason,
               const char *contentType, uintmax_t contentLength);
int sendText(int sock, int code, const char *reason, const char *body);
const char *mimeFor(const char *path);
int parsePort(const char *text, unsigned short *port);
#endif
