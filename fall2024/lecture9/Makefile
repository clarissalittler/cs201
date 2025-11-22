CC = gcc
CFLAGS = -Wall -g

# C source files (excluding commented versions)
C_SOURCES = $(filter-out %commented.c, $(wildcard *.c))
C_EXECUTABLES = $(C_SOURCES:.c=)

# Assembly source files (excluding commented versions)
S_SOURCES = $(filter-out %commented.s, $(wildcard *.s))
S_EXECUTABLES = $(S_SOURCES:.s=)

# All executables
EXECUTABLES = $(C_EXECUTABLES) $(S_EXECUTABLES)

# Files that need pthread (detected by grep)
PTHREAD_FILES = badthreads forkCounter

# Default target: build all executables
all: $(EXECUTABLES)

# Special rule for pthread programs
$(PTHREAD_FILES): %: %.c
	$(CC) $(CFLAGS) -pthread $< -o $@

# Pattern rule: compile C files
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Pattern rule: compile assembly files
%: %.s
	$(CC) $(CFLAGS) $< -o $@

# Clean up compiled files
clean:
	rm -f $(EXECUTABLES)

# Phony targets
.PHONY: all clean
