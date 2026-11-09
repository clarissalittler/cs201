CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g
LDLIBS = -pthread
PROGRAMS = echoServerThread webServer webClient

all: $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
