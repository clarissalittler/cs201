# Makefile for Socket Programming Exercises

CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
TARGETS = exercise1 exercise2 exercise3 exercise4 exercise5

all: $(TARGETS)

exercise1: exercise1.c
	$(CC) $(CFLAGS) -o $@ $<

exercise2: exercise2.c
	$(CC) $(CFLAGS) -o $@ $<

exercise3: exercise3.c
	$(CC) $(CFLAGS) -o $@ $<

exercise4: exercise4.c
	$(CC) $(CFLAGS) -o $@ $<

exercise5: exercise5.c
	$(CC) $(CFLAGS) -o $@ $<

# Test targets for convenience
test1: exercise1
	@echo "Starting echo server on port 8080..."
	@echo "Test with: echo 'Hello, Server!' | nc localhost 8080"
	./exercise1

test2: exercise2
	@echo "Starting client (make sure server is running)..."
	./exercise2

test3: exercise3
	@echo "Starting multi-client server..."
	@echo "Test with multiple: nc localhost 8080"
	./exercise3

test4: exercise4
	@echo "Starting concurrent server..."
	@echo "Test with multiple simultaneous: nc localhost 8080"
	./exercise4

test5: exercise5
	@echo "Starting chat server..."
	@echo "Connect with: nc localhost 8080"
	@echo "Commands: /list, /msg <user> <message>, /quit"
	./exercise5

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean test1 test2 test3 test4 test5