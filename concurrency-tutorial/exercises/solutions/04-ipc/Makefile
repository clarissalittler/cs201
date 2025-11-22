CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS_PTHREAD = -pthread
LDFLAGS_RT = -lrt
LDFLAGS_RT_PTHREAD = -lrt -pthread

# Solution targets
EXERCISE1 = exercise1_solution
EXERCISE2_SERVER = exercise2_server_solution
EXERCISE2_CLIENT = exercise2_client_solution
EXERCISE3_SENDER = exercise3_sender_solution
EXERCISE3_RECEIVER = exercise3_receiver_solution
EXERCISE4_PRODUCER = exercise4_producer_solution
EXERCISE4_CONSUMER = exercise4_consumer_solution
EXERCISE5_SERVER = exercise5_server_solution
EXERCISE5_CLIENT = exercise5_client_solution

ALL_TARGETS = $(EXERCISE1) $(EXERCISE2_SERVER) $(EXERCISE2_CLIENT) \
              $(EXERCISE3_SENDER) $(EXERCISE3_RECEIVER) \
              $(EXERCISE4_PRODUCER) $(EXERCISE4_CONSUMER) \
              $(EXERCISE5_SERVER) $(EXERCISE5_CLIENT)

# Default target
all: $(ALL_TARGETS)

# Exercise 1: Anonymous pipes
$(EXERCISE1): exercise1_solution.c
	$(CC) $(CFLAGS) $< -o $@

# Exercise 2: Named pipes (FIFOs)
$(EXERCISE2_SERVER): exercise2_server_solution.c
	$(CC) $(CFLAGS) $< -o $@

$(EXERCISE2_CLIENT): exercise2_client_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_PTHREAD)

# Exercise 3: Message queues
$(EXERCISE3_SENDER): exercise3_sender_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_RT)

$(EXERCISE3_RECEIVER): exercise3_receiver_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_RT)

# Exercise 4: Shared memory
$(EXERCISE4_PRODUCER): exercise4_producer_solution.c exercise4_shared_solution.h
	$(CC) $(CFLAGS) exercise4_producer_solution.c -o $@ $(LDFLAGS_RT_PTHREAD)

$(EXERCISE4_CONSUMER): exercise4_consumer_solution.c exercise4_shared_solution.h
	$(CC) $(CFLAGS) exercise4_consumer_solution.c -o $@ $(LDFLAGS_RT_PTHREAD)

# Exercise 5: Unix domain sockets
$(EXERCISE5_SERVER): exercise5_server_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_PTHREAD)

$(EXERCISE5_CLIENT): exercise5_client_solution.c
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

# Test target (compile and verify)
test: all
	@echo "All IPC exercise solutions compiled successfully!"
	@echo ""
	@echo "To run the solutions:"
	@echo "  Exercise 1: ./exercise1_solution"
	@echo "  Exercise 2: ./exercise2_server_solution (terminal 1), ./exercise2_client_solution (terminal 2)"
	@echo "  Exercise 3: ./exercise3_receiver_solution (terminal 1), ./exercise3_sender_solution (terminal 2)"
	@echo "  Exercise 4: ./exercise4_consumer_solution (terminal 1), ./exercise4_producer_solution (terminal 2)"
	@echo "  Exercise 5: ./exercise5_server_solution (terminal 1), ./exercise5_client_solution (terminal 2+)"

.PHONY: all clean test exercise1 exercise2 exercise3 exercise4 exercise5
