AS = as
ASFLAGS = -g
LD = ld
PROGRAMS = readWriteTest

all: $(PROGRAMS)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

readWriteTest: readWriteTest.o readInt.o writeInt.o
	$(LD) $(LDFLAGS) $^ -o $@

fork_lifecycle: fork_lifecycle.c
	$(CC) -std=c17 -Wall -Wextra -g $< -o $@

clean:
	rm -f $(PROGRAMS) *.o fork_lifecycle

.PHONY: all clean
