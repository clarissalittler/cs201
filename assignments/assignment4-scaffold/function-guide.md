# A Field Guide to the Functions You'll Need

This is a tour of the C standard library functions (and a couple of socket calls) that you'll probably reach for while filling out `handleRequest()` in `httpd.c`. None of this is required reading — if you already know `fread` you can skip past it — but it should save you a trip to the man pages.

You can pull up the full man page for any of these with `man 3 fread`, `man 2 read`, etc. The number is the section: section 2 is system calls, section 3 is library functions.

## Reading from and writing to the socket

A socket, once it's connected, behaves a lot like a file descriptor. You can `read()` and `write()` to it the same way you would a file you opened with `open()`.

### `read`
```c
#include <unistd.h>
ssize_t read(int fd, void* buf, size_t count);
```
Reads up to `count` bytes from `fd` into `buf`. Returns the number of bytes actually read, which may be **less than** what you asked for — that's normal, not an error. Returns 0 on end-of-file (the client closed the connection) and -1 on error.

For this assignment you can get away with a single `read()` because we're assuming the request fits in a 4kb buffer. Don't forget to leave room for a null terminator if you want to treat the buffer like a C string:

```c
char buf[BUF_SIZE];
ssize_t n = read(sock, buf, BUF_SIZE - 1);
if (n <= 0) return;
buf[n] = '\0';
```

### `write`
```c
#include <unistd.h>
ssize_t write(int fd, const void* buf, size_t count);
```
Writes `count` bytes from `buf` to `fd`. Like `read`, it might write fewer bytes than you asked for — in a more robust server you'd loop until everything is written, but for this assignment a single call is usually fine.

```c
write(sock, body, strlen(body));
```

## Parsing the request line

The first line of an HTTP request looks like `GET /index.html HTTP/1.0\r\n`. You need the method (probably just to confirm it's `GET`) and the path.

### `sscanf`
```c
#include <stdio.h>
int sscanf(const char* str, const char* format, ...);
```
Pulls formatted values out of a string. The `%s` conversion stops at whitespace, which is exactly what you want here:

```c
char method[16], path[256];
if (sscanf(buf, "%15s %255s", method, path) != 2) {
    /* malformed request */
    return;
}
```
The `15` and `255` are field-width limits — they prevent `sscanf` from overflowing your buffers if someone sends a weirdly long method name or path. The number is one less than the buffer size to leave room for the null terminator. Returns the number of fields successfully filled in.

### `strcmp`
```c
#include <string.h>
int strcmp(const char* a, const char* b);
```
Returns 0 if the two strings are equal, nonzero otherwise. Use it to check the method:

```c
if (strcmp(method, "GET") != 0) {
    /* not a GET — you could send a 405 or just bail */
}
```

### `strrchr`
```c
#include <string.h>
char* strrchr(const char* s, int c);
```
Finds the **last** occurrence of `c` in `s`. The provided `mimeFor()` uses this to find the file extension by searching for the last `.` in the path. You probably don't need to call it yourself — `mimeFor` does that for you — but it's handy to know about.

## Building the file path

You need to glue `DOCROOT` (`"./www"`) onto the URL path to get something like `./www/about.html`.

### `snprintf`
```c
#include <stdio.h>
int snprintf(char* str, size_t size, const char* format, ...);
```
Like `printf`, but writes into a buffer instead of stdout, and **never** writes more than `size` bytes (including the null terminator). Returns the number of bytes it *would have* written if there were room.

```c
char fullpath[512];
snprintf(fullpath, sizeof(fullpath), "%s%s", DOCROOT, path);
```

This is the safer cousin of `sprintf` (which has no size limit and is a great way to get a buffer overflow). Use `snprintf` always.

A worthy paranoid touch: you should probably refuse paths that contain `..` to prevent someone from asking for `/../../etc/passwd`. A quick `strstr(path, "..")` check works.

## Reading the file

### `fopen` / `fclose`
```c
#include <stdio.h>
FILE* fopen(const char* path, const char* mode);
int fclose(FILE* stream);
```
`fopen` returns a `FILE*` on success or `NULL` on failure (file doesn't exist, no permission, etc.). The mode `"rb"` means "read, binary" — the `b` matters on Windows but not Linux, but it's a good habit. If `fopen` returns `NULL`, send a 404.

Always `fclose` what you open, including on the error path before you return.

### `stat` — getting the file size before reading
```c
#include <sys/stat.h>
int stat(const char* path, struct stat* buf);
```
Fills in a `struct stat` with metadata about the file. Returns 0 on success, -1 on failure. The field you want is `st_size`:

```c
struct stat st;
if (stat(fullpath, &st) < 0) {
    /* couldn't stat — treat as 404 */
}
long size = st.st_size;
```

You need the size *before* sending the response header, because `Content-Length` is in the header.

(Alternative: open the file, `fseek(f, 0, SEEK_END)`, then `ftell(f)` to get the size, then `fseek(f, 0, SEEK_SET)` to rewind. Either approach works.)

### `fread`
```c
#include <stdio.h>
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
```
Reads up to `nmemb` items of `size` bytes each from `stream` into `ptr`. Returns the number of items actually read. For byte-by-byte reading just set `size` to 1:

```c
char chunk[4096];
size_t n;
while ((n = fread(chunk, 1, sizeof(chunk), f)) > 0) {
    write(sock, chunk, n);
}
```

This loop is the heart of serving the file: read a chunk, write it to the socket, repeat until `fread` returns 0.

## Sending the response

### `sendStatus` (provided)
```c
static void sendStatus(int sock, int code, const char* reason,
                       const char* contentType, long contentLength);
```
Writes the entire HTTP response header (status line, `Content-Type`, `Content-Length`, `Connection: close`, and the blank line separator) in one shot. Call it **once per response**, *before* sending the body.

```c
sendStatus(sock, 200, "OK", mimeFor(fullpath), size);
/* ... now write the body ... */
```

For a 404:
```c
const char* body = "<html><body><h1>404 Not Found</h1></body></html>";
sendStatus(sock, 404, "Not Found", "text/html", strlen(body));
write(sock, body, strlen(body));
```

### `mimeFor` (provided)
```c
static const char* mimeFor(const char* path);
```
Returns a content-type string based on the file extension. Falls back to `application/octet-stream` for unknown extensions. Just feed it the path you used to open the file. Extend the table inside `mimeFor` if you want to support more types for fun.

## Memory bookkeeping

You probably won't need to allocate much yourself — most of the buffers in `handleRequest()` can live on the stack as plain arrays. But if you do call `malloc`, every `malloc` needs a matching `free`, including on error paths. Same goes for `fopen` and `fclose`.

The provided `clientThread` already takes care of `free`ing the int it gets passed and `close`ing the socket after `handleRequest` returns, so you don't need to worry about either of those inside `handleRequest`.
