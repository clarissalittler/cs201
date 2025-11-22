CC = gcc
CFLAGS = -Wall -g

# List of C source files
SOURCES = echoserver.c firstsocket.c webby.c

# Generate executable names from source files
EXECUTABLES = $(SOURCES:.c=)

# Default target: build all executables
all: $(EXECUTABLES)

# Pattern rule: compile each .c file to an executable
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean up compiled files
clean:
	rm -f $(EXECUTABLES)

# Phony targets
.PHONY: all clean
