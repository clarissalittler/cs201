CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g -O0
PROGRAMS = integer_flags bitmunger1Corrected

all: $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
