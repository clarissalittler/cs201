CC = gcc
CFLAGS = -Wall -g -pthread

# List of C source files
SOURCES = dead1.c mut1.c mut2.c mutSem.c philo1.c philo2.c philo3.c philo4.c philo5.c sem1.c sem2.c

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
