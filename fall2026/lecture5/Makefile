AS = as
ASFLAGS = -g
LD = ld
PROGRAMS = cmp1 sumLoop addArray3

all: $(PROGRAMS)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

cmp1: cmp1.o
	$(LD) $(LDFLAGS) $^ -o $@

sumLoop: sumLoop.o
	$(LD) $(LDFLAGS) $^ -o $@

addArray3: addArray3.o
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(PROGRAMS) *.o

.PHONY: all clean
