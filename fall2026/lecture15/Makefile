CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g
LDLIBS = -pthread
SERVER_PROGRAMS = echoServerThread webServer webClient
FD_PROGRAMS = fdStandard fdCat fdDevices fdTable fdPipe fdRedirect
STDIO_PROGRAMS = stdioOpen stdioLines stdioChars stdioFormatted stdioBinary \
                 stdioSeek stdioBuffering stdioFileno procSelf fdopenServer
PROGRAMS = $(SERVER_PROGRAMS) $(FD_PROGRAMS) $(STDIO_PROGRAMS)

# files the examples create when you run them
OUTPUTS = out.txt err.txt listing.txt scores.txt points.bin wrapped.txt

all: $(PROGRAMS)

files: $(FD_PROGRAMS) $(STDIO_PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS) $(OUTPUTS)

.PHONY: all files clean
