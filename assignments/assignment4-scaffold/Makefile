CC = gcc
CFLAGS = -Wall -Wextra -g

httpd: httpd.c
	$(CC) $(CFLAGS) -pthread $< -o $@

clean:
	rm -f httpd

.PHONY: clean
