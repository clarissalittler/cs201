CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g
LDLIBS = -pthread
PROGRAMS = fdStandard fdCat fdDevices fdTable fdPipe fdRedirect pipe namedSem \
           argv openfile1 openfile2 readfile1 writefile1 writefile2 \
           directory1 lstatTest

# files the examples create when you run them
OUTPUTS = out.txt err.txt listing.txt

all: $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS) $(OUTPUTS)

.PHONY: all clean
