CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g -O0
PROGRAMS = floatmunger1Corrected floatlimit

all: $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
