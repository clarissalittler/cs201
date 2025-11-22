CC = gcc
CFLAGS = -Wall -g

# List of C source files (excluding .org files and text files)
SOURCES = argvtest.c directory1.c fileFunc1.c fileFunc2.c fileFunc3.c fileFunc4.c \
          fopen.c lineEditor.c openFile1.c openFile2.c openFile3.c openFileAlt.c storeFile.c

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
