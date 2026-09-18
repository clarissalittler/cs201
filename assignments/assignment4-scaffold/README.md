# Assignment 4 starter

Your task is to complete `handleRequest` in `httpd.c`. Start with a fixed response, then serve files from `www` as described in Assignment 4.

```sh
make
./httpd
```

In another terminal on the same machine:

```sh
curl -i http://localhost:8080/
python3 test_server.py
```

The placeholder returns **501 Not Implemented**. File-serving tests should fail until you finish the handler. The instructor-supplied helper tests, run with `make test-helpers`, should pass immediately.

If port 8080 is occupied, use `./httpd 18080` and `python3 test_server.py --port 18080`. If you use SSH, run the client in a second SSH session on the server. The server listens on loopback.

Read [the function guide](function-guide.md) for the helper contracts, file handling, ownership, and supported request format. You need a C compiler, Make, POSIX threads, and Python 3. No Python packages are required.

`www/_tests` contains instructor-provided test data. Keep those files unchanged; feel free to customize `www/index.html` and add your own files. Keep `www` free of symlinks and avoid changing files during tests. This is a controlled teaching exercise rather than a production web server.
