CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

# Exclude pedagogical commented variants from default builds.
C_SOURCES = $(filter-out %commented.c %_commented.c, $(wildcard *.c))
EXECUTABLES = $(C_SOURCES:.c=)

all: $(EXECUTABLES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(EXECUTABLES)

.PHONY: all clean
