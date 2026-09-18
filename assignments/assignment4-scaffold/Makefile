CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
PORT = 8080

httpd: httpd.c http_support.c http_support.h
	$(CC) $(CFLAGS) httpd.c http_support.c -pthread -o $@

test-helpers: tests/test_helpers.c http_support.c http_support.h
	$(CC) $(CFLAGS) tests/test_helpers.c -o test_helpers
	./test_helpers

# Run the server in another terminal first. These check YOUR finished handler.
test:
	python3 test_server.py --port $(PORT)

clean:
	rm -f httpd test_helpers

.PHONY: clean test test-helpers
