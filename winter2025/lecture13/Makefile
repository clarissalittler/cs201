# Makefile for Winter 2025 Lecture 13 - IPC Examples
# Compiles all IPC examples with proper flags

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lrt -pthread

# Message queue examples
MQ_TARGETS = task_distributor worker

# Shared memory examples
SHM_TARGETS = sharedMemprod sharedMemcons shm_creator shm_client

# Named pipe examples
FIFO_TARGETS = chat_server chat_client

# Socket examples
SOCK_TARGETS = fileserver fileclient

# All targets
TARGETS = $(MQ_TARGETS) $(SHM_TARGETS) $(FIFO_TARGETS) $(SOCK_TARGETS)

# Default target: build all examples
all: $(TARGETS)

# Message queue examples
task_distributor: task_distributor.c
	$(CC) $(CFLAGS) -o $@ $<

worker: worker.c
	$(CC) $(CFLAGS) -o $@ $<

# Shared memory examples
sharedMemprod: sharedMemprod.c sharedstruct.h
	$(CC) $(CFLAGS) -o $@ $< -lrt -pthread

sharedMemcons: sharedMemcons.c sharedstruct.h
	$(CC) $(CFLAGS) -o $@ $< -lrt -pthread

shm_creator: shm_creator.c
	$(CC) $(CFLAGS) -o $@ $< -lrt -pthread

shm_client: shm_client.c
	$(CC) $(CFLAGS) -o $@ $< -lrt -pthread

# Named pipe examples
chat_server: chat_server.c
	$(CC) $(CFLAGS) -o $@ $<

chat_client: chat_client.c
	$(CC) $(CFLAGS) -o $@ $<

# Socket examples
fileserver: fileserver.c
	$(CC) $(CFLAGS) -o $@ $< -pthread

fileclient: fileclient.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean up compiled files and FIFOs
clean:
	rm -f $(TARGETS)
	rm -f fifo_server fifo_client

# Phony targets (not actual files)
.PHONY: all clean
