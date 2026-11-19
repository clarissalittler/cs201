# A field guide to the server scaffold

Start in `handleRequest` in `httpd.c`. This is where you choose what a GET request receives. The scaffold supplies socket setup, worker threads, request validation, and helpers for reading and writing bytes.

## Getting started

Unzip the Assignment 4 starter and run these commands from its directory.

```sh
make
./httpd
```

The unfinished handler returns `501 Not Implemented`. Try replacing it with a fixed response using `sendText`. Once you can see your response in a client, try using the request path to decide what to send.

From another terminal on the same machine, run `curl -i http://127.0.0.1:8080/`. If you're working over SSH, use a second SSH session on that host. To start the server on a different port, try `./httpd 18080` and update the port in your request. Stop the server with Ctrl-C.

The starter binds to loopback so you can test it locally. Run `make test-helpers` to check the supplied helper routines independently of your handler.

## What arrives in your handler

```c
static void handleRequest(int sock, const Request *request)
```

`sock` is the connected client's socket. `request->path` is the validated URL path, such as `/` or `/story/forest`. It is a C string that starts with `/`. Use `strcmp` to match a specific path. The request belongs to this worker and remains valid until your handler returns.

The wrapper reads the complete header and validates it before calling you. It accepts `GET` with `HTTP/1.0` or `HTTP/1.1`. Headers can occupy at most 4096 bytes including the final `\r\n\r\n`; the wrapper allocates an extra byte for the string terminator. Paths have at most 255 characters and use slash-separated names containing ASCII letters, digits, underscores, hyphens, and periods. `/` is allowed; empty segments, trailing slashes on other paths, `.` and `..` segments, spaces, percent-encoding, backslashes, fragments, and query strings are rejected.

The wrapper also rejects request bodies and transfer encoding. A single `Content-Length: 0` is accepted. Requests outside the supported format get a small error response before they reach your handler. A project that takes an input can use a path like `/weather/portland` with the existing parser.

## Sending headers and bodies

```c
int sendStatus(int sock, int code, const char *reason,
               const char *contentType, uintmax_t contentLength);
int sendAll(int sock, const void *data, size_t length);
int sendText(int sock, int code, const char *reason, const char *body);
```

All three sending functions return `0` on success and `-1` on failure. If sending fails, stop trying to send that response and clean up your resources. Don't send a second status line after part of the first response has already gone out.

`sendText` calculates the length of a C string and sends it with `Content-Type: text/html`. You can use it for a fixed HTML body.

```c
if (sendText(sock, 200, "OK", "<h1>Hello from my server!</h1>\n") < 0) {
    return;
}
```

For a body with a different content type, such as `text/plain` or `application/json`, call `sendStatus` with the type and byte length, then call `sendAll` with the body. `sendStatus` sends the status line, `Content-Type`, `Content-Length`, `Connection: close`, and the blank line before the body. If you build a body with `snprintf`, check for a negative result or a result at least as large as the buffer before using that result as a byte count.

## Why the I/O helpers loop

TCP provides a stream of bytes. Even if the whole header fits in the buffer, it can arrive in several pieces. `readHeader` keeps reading until it sees the blank line or reaches the limit. It does not wait for the client to close the connection; the client is usually waiting for your response!

A successful `write` can also send fewer bytes than requested. `sendAll` tracks what remains and retries interrupted writes. `sendStatus` and `sendText` use it too. The server ignores SIGPIPE so that a client disconnecting mid-response does not terminate the entire process. A failed write is still an error your handler needs to handle.

## Who cleans up what?

The worker wrapper frees its argument and closes the connected socket after your handler returns. Your handler closes files it opens and frees memory it allocates. Plain local arrays are released when the function returns.

Each connection has a separate `Request`, header buffer, and call to the handler. Local buffers let each worker build a response independently. If you add shared mutable data, decide how concurrent requests can access it safely.

## Optional example: Serving files

If you want a file server, one design is to map `/` to `/index.html` and join each resulting path to `DOCROOT` (`"./www"`). For example, `/about.txt` becomes `./www/about.txt`. Check for errors or truncation if you join the paths with `snprintf`. Use a controlled document root free of symbolic links, and keep files unchanged during a download.

Open the file with `fopen(path, "rb")` and check whether it returns `NULL`. After a successful open, call `fstat(fileno(file), &st)` to get information about the open file, and check the result before using `st`. Use `S_ISREG(st.st_mode)` to check that it's a regular file. The response length comes from `st.st_size`. Check that it's nonnegative before converting it to `uintmax_t`. Close any successfully opened file with `fclose`, including on error paths.

Use `mimeFor(path)` for the file's content type. It covers HTML, plain text, CSS, PNG, and JPEG and uses `application/octet-stream` for unknown extensions. On success, send the headers, then use `fread(buffer, 1, sizeof(buffer), file)` in a loop. Send exactly the number of bytes each call returns, including any zero bytes in binary files. Check `ferror(file)` when reading stops to distinguish a read error from ordinary EOF.

If you follow this design, the included `www` directory gives you sample files and test fixtures. With the server running, `make test-files` runs the optional Python 3 checks. Use `make test-files PORT=18080` for a different port. These checks expect both `/` and `/index.html` to serve `www/index.html`, all fixtures under `www/_tests` to come back byte for byte, and missing files and directory paths to return a `404` with a nonempty HTML body. They also exercise concurrent clients and recovery from a disconnect. For other projects, test the behavior you designed with `curl` or a client of your choice.

## References

You can read the [Linux read manual](https://man7.org/linux/man-pages/man2/read.2.html), [write manual](https://man7.org/linux/man-pages/man2/write.2.html), or [HTTP message framing specification](https://www.rfc-editor.org/rfc/rfc9112.html) if you'd like to explore why these details matter.
