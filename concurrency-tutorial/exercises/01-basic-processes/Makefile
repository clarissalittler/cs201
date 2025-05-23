CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

# Exercise targets
EXERCISES = exercise1 exercise2 exercise3 exercise4 exercise5

# Default target
all: $(EXERCISES)

# Individual exercise targets
exercise1: exercise1.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

exercise2: exercise2.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

exercise3: exercise3.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

exercise4: exercise4.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

exercise5: exercise5.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Clean target
clean:
	rm -f $(EXERCISES)

# Test target (compile only, don't run)
test: all
	@echo "All exercises compiled successfully!"

.PHONY: all clean test