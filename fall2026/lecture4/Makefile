AS = as
ASFLAGS = -g
LD = ld
PROGRAMS = firstfixed firstadd hello

all: $(PROGRAMS)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

firstfixed: firstfixed.o
	$(LD) $(LDFLAGS) $^ -o $@

firstadd: firstadd.o
	$(LD) $(LDFLAGS) $^ -o $@

hello: hello.o
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(PROGRAMS) *.o

.PHONY: all clean
