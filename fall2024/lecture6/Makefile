CC = gcc
CFLAGS = -Wall -g
AS = as
LD = ld

# C source files (excluding commented versions)
C_SOURCES = $(filter-out %commented.c, $(wildcard *.c))
C_EXECUTABLES = $(C_SOURCES:.c=)

# Assembly source files (excluding commented versions)
S_SOURCES = $(filter-out %commented.s, $(wildcard *.s))
S_EXECUTABLES = $(S_SOURCES:.s=)

# All executables
EXECUTABLES = $(C_EXECUTABLES) $(S_EXECUTABLES)

# Default target: build all executables
all: $(EXECUTABLES)

# Pattern rule: compile C files
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Pattern rule: compile assembly files
%: %.s
	$(AS) $< -o $@.o
	$(LD) $@.o -o $@
	rm -f $@.o

# Clean up compiled files
clean:
	rm -f $(EXECUTABLES)

# Phony targets
.PHONY: all clean
