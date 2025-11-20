# Makefile for IPC Lecture Examples
# Compiles all IPC lecture examples with proper flags

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lrt -pthread

# Message queue examples
MQ_TARGETS = msgqSender1 msgqReceiver1

# Shared memory examples
SHM_TARGETS = sharedMemprod sharedMemcons

# Socket examples
SOCK_TARGETS = echoserver echoclient fileserver fileclient

# Named pipe examples
FIFO_TARGETS = namedPipesServer1 namedPipesClient1

# All targets
TARGETS = $(MQ_TARGETS) $(SHM_TARGETS) $(SOCK_TARGETS) $(FIFO_TARGETS)

# Default target: build all examples
all: $(TARGETS)

# Message queue examples (need -lrt for POSIX message queues)
msgqSender1: msgqSender1.c
	$(CC) $(CFLAGS) -o $@ $< -lrt

msgqReceiver1: msgqReceiver1.c
	$(CC) $(CFLAGS) -o $@ $< -lrt

# Shared memory examples (need -lrt -pthread for POSIX shared memory and semaphores)
sharedMemprod: sharedMemprod.c sharedstruct.h
	$(CC) $(CFLAGS) -o $@ $< -lrt -pthread

sharedMemcons: sharedMemcons.c sharedstruct.h
	$(CC) $(CFLAGS) -o $@ $< -lrt -pthread

# Socket examples
echoserver: echoserver.c
	$(CC) $(CFLAGS) -o $@ $< -pthread

echoclient: echoclient.c
	$(CC) $(CFLAGS) -o $@ $<

fileserver: fileserver.c
	$(CC) $(CFLAGS) -o $@ $< -pthread

fileclient: fileclient.c
	$(CC) $(CFLAGS) -o $@ $<

# Named pipe examples
namedPipesServer1: namedPipesServer1.c
	$(CC) $(CFLAGS) -o $@ $<

namedPipesClient1: namedPipesClient1.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean up compiled files
clean:
	rm -f $(TARGETS)

# Phony targets (not actual files)
.PHONY: all clean
