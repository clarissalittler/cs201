CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g
PROGRAMS = fdStandard fdCat fdDevices fdTable fdPipe fdRedirect \
           stdioOpen stdioLines stdioChars stdioFormatted stdioBinary \
           stdioSeek stdioBuffering stdioFileno procSelf \
           argv openfile1 openfile2 openfileagain readfile1 writefile1 \
           writefile2 weirdcopy directory1 lstatTest lineEditor

# files the examples create when you run them
OUTPUTS = out.txt err.txt listing.txt scores.txt points.bin wrapped.txt

all: $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS) $(OUTPUTS)

.PHONY: all clean
