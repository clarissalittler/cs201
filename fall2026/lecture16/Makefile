CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -g
PROGRAMS = stdioOpen stdioLines stdioChars stdioFormatted stdioBinary \
           stdioSeek stdioBuffering stdioFileno procSelf \
           openfileagain weirdcopy lineEditor

# files the examples create when you run them
OUTPUTS = scores.txt points.bin wrapped.txt

all: $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGRAMS) $(OUTPUTS)

.PHONY: all clean
