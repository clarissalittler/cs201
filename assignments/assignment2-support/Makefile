AS = as
LD = ld
ASFLAGS = --gdwarf-4

sum5: sum5.o readInt.o writeInt.o
	$(LD) -o $@ $^

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f sum5 *.o

.PHONY: clean
