# A field guide to the server scaffold

Your job is to fill in `handleRequest` in `httpd.c`. The socket setup, thread creation, request validation, and routines for reading and writing a stream of bytes are already provided. You can read their code in `http_support.c`, but you don't have to rewrite it.

## Getting started

Download and unzip the Assignment 4 starter, then open a terminal in its directory. Build and run it with:

```sh
make
./httpd
```

The unfinished handler returns `501 Not Implemented`. That's expected! Start by replacing it with a fixed response, then work through the assignment's file-serving steps.

To use a different port, try `./httpd 18080`. In another terminal **on the same machine**, test it with `curl -i http://localhost:18080/`. If you're connected to the Linux server through SSH, that second terminal should be a second SSH session. Run `python3 test_server.py --port 18080` to check your finished handler. The default is port 8080.

The server binds to loopback, so clients run on that same machine. This is a small teaching server for a controlled directory of files, not a server to expose to the public Internet. Stop it with Ctrl-C. Don't change a file while a test is downloading it.

## What arrives in your handler

```c
static void handleRequest(int sock, const Request *request)
```

`sock` is the connected client's socket. `request->path` is the validated URL path, such as `/`, `/about.txt`, or `/_tests/pixel.png`. It is a C string and still starts with `/`. The request belongs to this worker and remains valid until your handler returns.

The wrapper reads the complete header and validates it before calling you. It accepts `GET` with `HTTP/1.0` or `HTTP/1.1`. Headers can occupy at most 4096 bytes including the final `\r\n\r\n`; the wrapper allocates an extra byte for the string terminator. Paths have at most 255 characters and use slash-separated names containing ASCII letters, digits, underscores, hyphens, and periods. `/` is allowed; empty segments, trailing slashes on other paths, `.` and `..` segments, spaces, percent-encoding, backslashes, fragments, and query strings are rejected.

The wrapper also rejects request bodies and transfer encoding. A single `Content-Length: 0` is accepted. Unsupported requests get a small error response without entering your handler. This intentionally limited parser does not implement every HTTP rule.

## Mapping the path to a file

Map `/` to `/index.html`. For another accepted path, join `DOCROOT` (`"./www"`) and `request->path`, so `/about.txt` becomes `./www/about.txt`.

`snprintf` writes formatted text into a bounded buffer. It returns the number of characters it would have written, excluding the string terminator. Check for a negative result or a result greater than or equal to the buffer size; either means you cannot use the result as the intended complete path.

Keep the document root free of symbolic links. The validator checks URL syntax, not the entire filesystem: a symlink could point somewhere else even when its name looks fine.

## Opening and inspecting a file

Use `fopen(path, "rb")` to open a file for binary reading, and check whether it returned `NULL`. Use `fclose` to close any file you successfully opened, even if a later step fails.

`fstat(fileno(file), &st)` gives you information about the file you actually opened. Check the return value before using `st`. `S_ISREG(st.st_mode)` tells you whether it is a regular file, and `st.st_size` gives its size in bytes. For this assignment, respond with a 404 if the file cannot be opened or inspected, or is not a regular file. Check that its size is nonnegative before converting it to `uintmax_t` for the response header.

The headers needed for those calls are already included in `httpd.c`.

## Sending headers and bodies

```c
int sendStatus(int sock, int code, const char *reason,
               const char *contentType, uintmax_t contentLength);
int sendAll(int sock, const void *data, size_t length);
int sendText(int sock, int code, const char *reason, const char *body);
const char *mimeFor(const char *path);
```

All three sending functions return `0` on success and `-1` on failure. If sending fails, stop trying to send that response and clean up your resources. Don't send a second status line after part of the first response has already gone out.

`sendStatus` sends the status line, `Content-Type`, `Content-Length`, `Connection: close`, and the blank line separating headers from the body. Call it once before sending file bytes. `mimeFor` returns the content type for a filename, falling back to `application/octet-stream` for unrecognized extensions.

For a short text or HTML response, `sendText` calculates the body length and sends both headers and body. For example:

```c
if (sendText(sock, 200, "OK", "Hello from my server!\n") < 0) {
    return;
}
```

This sends a `text/html` response. It is useful for the first checkpoint and for a short 404 body. For a file, use `sendStatus` with the file's byte size, followed by calls to `sendAll`.

`fread(buffer, 1, sizeof(buffer), file)` reads up to one buffer of bytes and returns how many it read. Send exactly that many bytes, then repeat until it returns zero or sending fails. Check `ferror(file)` to distinguish a read error from ordinary EOF. Don't use `strlen` on file contents: a PNG or JPEG can contain zero bytes that are part of the file.

## Why the I/O helpers loop

TCP provides a stream of bytes. Even if the whole header fits in the buffer, it can arrive in several pieces. `readHeader` keeps reading until it sees the blank line or reaches the limit. It does not wait for the client to close the connection; the client is usually waiting for your response!

A successful `write` can also send fewer bytes than requested. `sendAll` tracks what remains and retries interrupted writes. `sendStatus` and `sendText` use it too. The server ignores SIGPIPE so that a client disconnecting mid-response does not terminate the entire process. A failed write is still an error your handler needs to handle.

## Who cleans up what?

The worker wrapper frees its argument and closes the connected socket after your handler returns. **Don't close that socket a second time in your handler.** Your handler closes files it opens and frees memory it allocates. Plain local arrays are released when the function returns.

Each connection has a separate `Request`, header buffer, and call to the handler. Keep your file and transfer buffer local too. A global mutable request buffer would let clients overwrite each other's work.

## Checking your work

With the server running, `make test` runs the Python client checks on port 8080. Use `make test PORT=18080` for another port. Python 3 is the only client dependency.

The tests check exact file bytes, headers, a missing file, a directory, fragmented requests, concurrent clients, and recovery after a client disconnects. The supplied `_tests` files are test inputs; don't edit them to make a failing check pass. You can edit `www/index.html` and add your own files.

`make test-helpers` checks only the instructor-supplied support routines, including simulated short writes and interruptions. These tests should pass before you implement your handler. The full client tests intentionally fail on the initial 501 placeholder.

## References

You can read the [Linux read manual](https://man7.org/linux/man-pages/man2/read.2.html), [write manual](https://man7.org/linux/man-pages/man2/write.2.html), or [HTTP message framing specification](https://www.rfc-editor.org/rfc/rfc9112.html) if you'd like to explore why these details matter.
