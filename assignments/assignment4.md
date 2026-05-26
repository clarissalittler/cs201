# CS 201 Final Project — A Tiny Web Server

For your last assignment, you'll build a small HTTP server in C that serves static files over the network. Once it's running, you'll be able to point your browser at `http://localhost:8080/` and see pages you put in the `www/` directory show up.

This should be a pretty doable project that involves a number of the skills that we've been practicing over this term.
## Goals

The goal is to make a very simple web-server that will serve files. When you open a browser and point it at `http://localhost:8080/` you should see the file `www/index.html` served. If you navigate to `http://localhost:8080/some_other_file.html` and see that file. If you ask for a file that doesn't exist you should get a 404 error rather than a crash.

What does this mean? It means you have to:

- Listen for TCP connections on port 8080.
- Accept connections in a loop. Each connection runs in its own thread (the scaffold sets this up).
- Read the request from the client and parse the first line, which looks like `GET /path HTTP/1.0\r\n`.
- Map `/` to `/index.html`. Map any other path to a file inside the `www/` directory.
- If the file exists, send a `200 OK` response with a correct `Content-Type` header (based on the file extension) and a correct `Content-Length` header (the byte size of the file). Then send the file's bytes.
- If the file doesn't exist, send a `404 Not Found` response with a short HTML body.
- Handle at least these file types: `.html`, `.txt`, `.css`, `.png`, `.jpg`. The scaffold has a starter MIME table you can extend.

You only need to handle `GET` requests. You can assume the request
fits in a 4kb buffer. 

## Quick and dirty HTTP tutorial

An HTTP/1.0 request looks like this (the `\r\n` are CRLF line endings, and
there's a blank line at the end):

```
GET /index.html HTTP/1.0\r\n
Host: localhost:8080\r\n
\r\n
```

A successful response looks like this:

```
HTTP/1.0 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 137\r\n
Connection: close\r\n
\r\n
<html><body>...the bytes of the file...</body></html>
```

A 404 looks like this:

```
HTTP/1.0 404 Not Found\r\n
Content-Type: text/html\r\n
Content-Length: 48\r\n
Connection: close\r\n
\r\n
<html><body><h1>404 Not Found</h1></body></html>
```

After sending the response, close the connection.

## Scaffolding you can use

https://github.com/clarissalittler/cs201/tree/main/assignments/assignment4-scaffold

In the scaffold:

- `httpd.c` — The server 
- `Makefile` — `make` builds the server, `make clean` removes the binary.
- `www/` — a starter directory with a tiny `index.html`. Put any files you want
  to serve in here.

## Testing

The blessed thing about an HTTP project is that you don't have to write your own client! You can test with:

- A web browser pointed at `http://localhost:8080/`
- `curl http://localhost:8080/index.html`
- `curl -v http://localhost:8080/missing.html` — the `-v` shows headers
- (just for fun) `telnet localhost 8080` and then typing the request manually

Check that threading works! Open the page in multiple tabs, write a script that runs curl repeatedly as fast as possible, &c.

## Stretch goals

There are a couple of things you could try to add to your project for fun:

**Directory listings** When the requested path is a directory rather than a file, generate an HTML page listing the files in that directory, with each file name as a clickable link. Your first taste of generating HTML on the fly!

**Thread pool** Replace one-thread-per-connection with a constant sized pool of worker threads pulling connections from a shared work queue. This requires a mutex (to protect the queue itself) and learning about `pthread_cond_t` so you can efficiently wait while the queue is empty. Perfect if you're feeling a little *ambitious* and/or easily bored!
