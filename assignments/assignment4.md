# Assignment 4: Serving files over HTTP

For our last assignment, let's build something you can point a browser at! You'll finish a small HTTP server in C that serves files from a directory called `www`.

This brings together several things we've practiced: working with arrays of bytes, reading files, managing resources, and letting threads handle separate pieces of work.

Download the [Assignment 4 starter ZIP](downloads/assignment4-starter.zip), which includes the scaffold, [function guide](assignment4-scaffold/function-guide.md), test files, and test script.

## What you're building

The [supplied scaffold](assignment4-scaffold/) listens for connections and starts a thread for each client. Your part is `handleRequest`: use the validated path in `request->path` to work out what file the client wants and send back a response.

Your server should:

- Handle `GET` requests and send HTTP/1.0 responses, closing the connection after each response.
- Map `/` to `www/index.html` and other supported paths to files inside `www`.
- Send `200 OK`, the appropriate `Content-Type`, the file's byte length as `Content-Length`, and then the file's bytes when the file exists.
- Send `404 Not Found` with a short body when the file doesn't exist. Calculate the body's length from the body you're actually sending.
- Serve `.html`, `.txt`, `.css`, `.png`, and `.jpg` files. The scaffold provides a helper for the content types.
- Close any files you open and release any memory you allocate, including when something goes wrong.

We'll keep the supported requests small and simple: headers must fit in the scaffold's 4 KB limit, paths use ordinary unencoded names containing letters, digits, underscores, hyphens, and periods, separated by single slashes, and we don't need request bodies, query strings, or persistent connections. The supplied helpers handle reading a complete header, checking the supported path format, and sending all the requested bytes. Use those helpers and check their return values. The scaffold handles rejection of requests outside this limited format.

Keep the files you serve inside `www`, and don't put symbolic links in that directory. This project doesn't require implementing the full HTTP standard.

## Work through it in stages

**First, send a fixed response.** Have your handler return a short message, such as `Hello from my server!`, with the correct headers. Check that the advertised length matches the bytes you send. This gives you a small working program before you add file handling.

**Next, serve a file.** Make `/` return `www/index.html`, then support another path such as `/about.txt`. Add the missing-file response. Check file-opening and reading results so an error doesn't lead to using invalid data.

**Finally, check different files and clients.** Serve the required file types, including an image and a file larger than your transfer buffer. Use the number of bytes read from a file when sending each chunk. An image can contain zero bytes, so `strlen` won't tell you its size!

These are development checkpoints within one assignment. You can keep improving the same program as you go.

## Running and testing

Use the supplied `Makefile` to build the server:

```sh
make
./httpd
```

Run it from the scaffold directory so it can find `www`. In a second terminal on the **same machine**, try:

```sh
curl -i http://localhost:8080/
curl -i http://localhost:8080/missing.html
```

If you're working on the Linux server through SSH, open a second SSH session and run `curl` there. `localhost` means the machine running the client: a browser on your laptop won't reach the remote server just by visiting your laptop's `localhost` address. If you run the server locally, you can use your local browser directly.

If another student is using port 8080, run `./httpd 18080` to choose another port, then use that same port in your test commands. Run `python3 test_server.py` for the supplied checks, or `python3 test_server.py --port 18080` for another port. `make test-helpers` checks the supplied helper routines independently of your handler; those checks should pass from the start. The full server tests will fail until you replace the initial `501 Not Implemented` response.

Use the supplied test files and test script to check:

- `/` and an explicitly named file return the expected contents.
- A missing file returns a 404 response and the correct body length.
- Text, stylesheets, and images have the expected content types.
- An empty file, an image, and a file larger than the transfer buffer arrive intact.
- Several clients can fetch files at the same time and receive complete, correct responses.

Include the test report and one response whose headers you explain. If something doesn't pass, include what you tried and where you're stuck.

## Explain one piece

Walk us through one connection. Which thread handles it? Who closes the connected socket? Who closes the file? Which data belongs to that connection, and which data is shared?

The scaffold already starts the threads, but you should be able to explain why two requests don't accidentally use each other's request buffers or sockets.

## What to submit

Submit your source files and `Makefile`, any small files needed to reproduce your tests, build and run instructions, your test report, and your explanation. You don't need to submit the executable. The required server described above is enough for full credit.

If you'd like to go further, try directory listings or replace one-thread-per-connection with a fixed pool of workers. A thread pool would connect this project to Assignment 3's shared-work idea, with a condition variable to let workers wait for new connections. These are optional extensions.
