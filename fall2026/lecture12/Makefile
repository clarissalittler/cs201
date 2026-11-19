CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g -O0
LDLIBS = -pthread
PROGRAMS = prodcons semBouncer
MMAP_PROGRAMS = mmapAnon mmapfaults mmapRead mmapFile mmapFork mmapIPC mmapProtect

all: $(PROGRAMS)

mmap: $(MMAP_PROGRAMS)

check-mmap: mmap
	python3 check-mmap.py

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS) $(MMAP_PROGRAMS)

.PHONY: all mmap check-mmap clean
