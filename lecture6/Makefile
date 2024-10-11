# Makefile to compile all .s files with as and link them
.SUFFIXES:

# Variables
ASM_FILES := $(wildcard *.s)
OBJ_FILES := $(ASM_FILES:.s=.o)
EXECUTABLES := $(ASM_FILES:.s=)

# Default rule: compile and link all assembly files
all: $(EXECUTABLES)

# Rule to assemble and link each .s file
%: %.o
	ld $< -o $@

%.o: %.s
	as $< -o $@

# Clean up the object files and executables
clean:
	rm -f $(OBJ_FILES) $(EXECUTABLES)

# Phony target so that clean is always executed
.PHONY: all clean
