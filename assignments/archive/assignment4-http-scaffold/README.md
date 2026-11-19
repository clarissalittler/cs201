# Assignment 4 starter

Start in `handleRequest` in `httpd.c`. Send a fixed response first, then decide what you want your server to return for the GET paths you support. The scaffold supplies the listening socket, worker threads, request validation, and response helpers.

```sh
make
./httpd
```

Send a request from another terminal on the same machine.

```sh
curl -i http://localhost:8080/
```

The placeholder returns **501 Not Implemented**. Run `make test-helpers` to check the supplied helper routines. Once you've added your own behavior, use `curl` with the paths your server supports.

If port 8080 is occupied, use `./httpd 18080` and request that port instead. If you use SSH, run the client in a second SSH session on the server. The starter listens on loopback.

Read [the function guide](function-guide.md) for the supported request format, response helpers, and resource ownership. To build the starter you need a C compiler, Make, and POSIX threads.

If you choose to serve files, the guide has an optional walkthrough. The included `www` directory and `make test-files` (which uses Python 3) go with that walkthrough.
