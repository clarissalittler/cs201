CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g -O0
PROGRAMS = locality

all: $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

locality-O2: locality.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -O2 $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS) locality-O2

.PHONY: all clean
