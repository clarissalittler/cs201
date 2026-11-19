AS = as
ASFLAGS = -g
LD = ld
PROGRAMS = readWriteTest sumLoopArray fact

all: $(PROGRAMS)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

readWriteTest: readWriteTest.o readInt.o writeInt.o
	$(LD) $(LDFLAGS) $^ -o $@

sumLoopArray: sumLoopArray.o readInt.o writeInt.o
	$(LD) $(LDFLAGS) $^ -o $@

fact: fact.o readInt.o writeInt.o
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(PROGRAMS) *.o

.PHONY: all clean
