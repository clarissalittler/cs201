AS = as
ASFLAGS = -g
LD = ld
PROGRAMS = 02-basic-function fun1 fun2 funStack

all: $(PROGRAMS)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

02-basic-function: 02-basic-function.o
	$(LD) $(LDFLAGS) $^ -o $@

fun1: fun1.o
	$(LD) $(LDFLAGS) $^ -o $@

fun2: fun2.o
	$(LD) $(LDFLAGS) $^ -o $@

funStack: funStack.o
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(PROGRAMS) *.o

.PHONY: all clean
