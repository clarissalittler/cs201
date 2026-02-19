CC = gcc
CFLAGS = -Wall -g
AS = as
LD = ld

# C source files (excluding commented versions)
C_SOURCES = $(filter-out %commented.c, $(wildcard *.c))
C_EXECUTABLES = $(C_SOURCES:.c=)

# Assembly source files that are directly runnable (excluding helper routines).
S_SOURCES = $(filter-out %commented.s readInt.s writeInt.s writeIntBad.s, $(wildcard *.s))
S_EXECUTABLES = $(S_SOURCES:.s=)

# All executables
EXECUTABLES = $(C_EXECUTABLES) $(S_EXECUTABLES)

# Default target: build all executables
all: $(EXECUTABLES)

# echoInt depends on helper routines from separate assembly files.
echoInt: echoInt.s readInt.s writeInt.s
	$(AS) echoInt.s -o echoInt.o
	$(AS) readInt.s -o readInt.o
	$(AS) writeInt.s -o writeInt.o
	$(LD) echoInt.o readInt.o writeInt.o -o echoInt
	rm -f echoInt.o readInt.o writeInt.o

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
