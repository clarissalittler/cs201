# Makefile to compile all .s files with as and link them
.SUFFIXES:

# Variables
HELPER_SOURCES := countStr.s readInt3.s stolenWriter.s writeInt.s
ASM_FILES := $(filter-out %commented.s $(HELPER_SOURCES), $(wildcard *.s))
OBJ_FILES := $(ASM_FILES:.s=.o) $(HELPER_SOURCES:.s=.o)
EXECUTABLES := $(ASM_FILES:.s=)

# Default rule: compile and link all assembly files
all: $(EXECUTABLES)

# Programs that depend on helper routines defined in separate .s files.
fib: fib.o readInt3.o
	ld fib.o readInt3.o -o fib

helloAgain: helloAgain.o countStr.o
	ld helloAgain.o countStr.o -o helloAgain

localVars1: localVars1.o readInt3.o
	ld localVars1.o readInt3.o -o localVars1

mulDiv: mulDiv.o stolenWriter.o
	ld mulDiv.o stolenWriter.o -o mulDiv

writeTester: writeTester.o stolenWriter.o
	ld writeTester.o stolenWriter.o -o writeTester

# Rule to assemble and link each .s file
%: %.o
	ld $< -o $@

%.o: %.s
	as -g $< -o $@

# Clean up the object files and executables
clean:
	rm -f $(OBJ_FILES) $(EXECUTABLES)

# Phony target so that clean is always executed
.PHONY: all clean
