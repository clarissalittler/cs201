# Makefile for IPC Examples
# Compiles all IPC examples with proper flags

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS =

# Target executables
TARGETS = 01-pipes 02-named-pipes 03-shared-memory 04-socket-ipc

# Default target: build all examples
all: $(TARGETS)

# Individual targets
01-pipes: 01-pipes.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

02-named-pipes: 02-named-pipes.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

03-shared-memory: 03-shared-memory.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

04-socket-ipc: 04-socket-ipc.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Clean up compiled files
clean:
	rm -f $(TARGETS)

# Phony targets (not actual files)
.PHONY: all clean
