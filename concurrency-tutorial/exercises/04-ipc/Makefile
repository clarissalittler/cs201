CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS_PTHREAD = -pthread
LDFLAGS_RT = -lrt
LDFLAGS_RT_PTHREAD = -lrt -pthread

# Exercise targets
EXERCISE1 = exercise1
EXERCISE2_SERVER = exercise2_server
EXERCISE2_CLIENT = exercise2_client
EXERCISE3_SENDER = exercise3_sender
EXERCISE3_RECEIVER = exercise3_receiver
EXERCISE4_PRODUCER = exercise4_producer
EXERCISE4_CONSUMER = exercise4_consumer
EXERCISE5_SERVER = exercise5_server
EXERCISE5_CLIENT = exercise5_client

ALL_TARGETS = $(EXERCISE1) $(EXERCISE2_SERVER) $(EXERCISE2_CLIENT) \
              $(EXERCISE3_SENDER) $(EXERCISE3_RECEIVER) \
              $(EXERCISE4_PRODUCER) $(EXERCISE4_CONSUMER) \
              $(EXERCISE5_SERVER) $(EXERCISE5_CLIENT)

# Default target
all: $(ALL_TARGETS)

# Exercise 1: Anonymous pipes
$(EXERCISE1): exercise1.c
	$(CC) $(CFLAGS) $< -o $@

# Exercise 2: Named pipes (FIFOs)
$(EXERCISE2_SERVER): exercise2_server.c
	$(CC) $(CFLAGS) $< -o $@

$(EXERCISE2_CLIENT): exercise2_client.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_PTHREAD)

# Exercise 3: Message queues
$(EXERCISE3_SENDER): exercise3_sender.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_RT)

$(EXERCISE3_RECEIVER): exercise3_receiver.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_RT)

# Exercise 4: Shared memory
$(EXERCISE4_PRODUCER): exercise4_producer.c exercise4_shared.h
	$(CC) $(CFLAGS) exercise4_producer.c -o $@ $(LDFLAGS_RT_PTHREAD)

$(EXERCISE4_CONSUMER): exercise4_consumer.c exercise4_shared.h
	$(CC) $(CFLAGS) exercise4_consumer.c -o $@ $(LDFLAGS_RT_PTHREAD)

# Exercise 5: Unix domain sockets
$(EXERCISE5_SERVER): exercise5_server.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_PTHREAD)

$(EXERCISE5_CLIENT): exercise5_client.c
	$(CC) $(CFLAGS) $< -o $@

# Individual exercise targets for convenience
exercise1: $(EXERCISE1)
exercise2: $(EXERCISE2_SERVER) $(EXERCISE2_CLIENT)
exercise3: $(EXERCISE3_SENDER) $(EXERCISE3_RECEIVER)
exercise4: $(EXERCISE4_PRODUCER) $(EXERCISE4_CONSUMER)
exercise5: $(EXERCISE5_SERVER) $(EXERCISE5_CLIENT)

# Clean target
clean:
	rm -f $(ALL_TARGETS)
	rm -f client_to_server server_to_client  # Clean up FIFOs
	rm -f /tmp/ipc_exercise_socket            # Clean up socket file

# Test target (compile only, don't run)
test: all
	@echo "All IPC exercises compiled successfully!"

# Help target
help:
	@echo "Available targets:"
	@echo "  make all       - Compile all exercises"
	@echo "  make exercise1 - Compile exercise 1 (anonymous pipes)"
	@echo "  make exercise2 - Compile exercise 2 (named pipes)"
	@echo "  make exercise3 - Compile exercise 3 (message queues)"
	@echo "  make exercise4 - Compile exercise 4 (shared memory)"
	@echo "  make exercise5 - Compile exercise 5 (Unix domain sockets)"
	@echo "  make clean     - Remove compiled files and IPC artifacts"
	@echo "  make test      - Compile all and verify"

.PHONY: all clean test help exercise1 exercise2 exercise3 exercise4 exercise5
