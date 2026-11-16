CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g
LDLIBS = -pthread
PROGRAMS = step1HelloOnce step2HelloLoop step3EchoLoop step4EchoPoll step5EchoSelect step6Client \
           echoServerThread webServer webClient

all: $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
