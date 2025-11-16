# Makefile for Assembly Examples
# Use this to easily build examples with their dependencies

# Default target builds all examples
all: basics data control functions io advanced

# Build examples by category
basics:
	@echo "Building Basic Examples..."
	cd 01-basics && \
	as 01-hello-world.s -o hello-world.o && ld hello-world.o -o hello-world && \
	as 02-simple.s -o simple.o && ld simple.o -o simple && \
	as 03-simplest.s -o simplest.o && ld simplest.o -o simplest && \
	as 04-simpler.s -o simpler.o && ld simpler.o -o simpler

data:
	@echo "Building Data Examples..."
	cd 02-data && \
	as 01-array.s -o array.o && ld array.o -o array && \
	as 02-data-section.s -o data-section.o && ld data-section.o -o data-section && \
	as 03-data-types.s -o data-types.o && ld data-types.o -o data-types && \
	as 04-data-constants.s -o data-constants.o && ld data-constants.o -o data-constants

control:
	@echo "Building Control Flow Examples..."
	cd 03-control-flow && \
	as 01-for-loop.s -o for-loop.o && ld for-loop.o -o for-loop && \
	as 02-while-loop.s -o while-loop.o && ld while-loop.o -o while-loop && \
	as 03-comparisons.s -o comparisons.o && ld comparisons.o -o comparisons && \
	as 04-float-comparisons.s -o float-comparisons.o && ld float-comparisons.o -o float-comparisons

# First build the reusable IO functions
io_functions:
	@echo "Building I/O Functions..."
	cd 05-io && \
	as 02-read-integer.s -o readInt.o && \
	as 03-write-integer.s -o writeInt.o

# Build function examples (some depend on IO functions)
functions: io_functions
	@echo "Building Function Examples..."
	cd 04-functions && \
	as 02-basic-function.s -o basic-function.o && ld basic-function.o -o basic-function && \
	as 03-function-parameters.s -o function-parameters.o && ld function-parameters.o -o function-parameters && \
	as 01-fibonacci.s -o fibonacci.o && ld fibonacci.o ../05-io/readInt.o ../05-io/writeInt.o -o fibonacci

# Build IO examples
io: io_functions
	@echo "Building I/O Examples..."
	cd 05-io && \
	as 01-echo.s -o echo.o && ld echo.o -o echo && \
	as 04-echo-integer.s -o echo-integer.o && ld echo-integer.o readInt.o writeInt.o -o echo-integer

# Build advanced examples
advanced: io_functions
	@echo "Building Advanced Examples..."
	cd 06-advanced && \
	as 01-avx512.s -o avx512.o && ld avx512.o -o avx512 && \
	as 03-floating-point.s -o floating-point.o && ld floating-point.o -o floating-point && \
	as 02-multiplication-division.s -o multiplication-division.o && ld multiplication-division.o ../05-io/writeInt.o -o multiplication-division && \
	as 04-stack-manipulation.s -o stack-manipulation.o && ld stack-manipulation.o ../05-io/readInt.o ../05-io/writeInt.o -o stack-manipulation

# Clean all built files
clean:
	@echo "Cleaning up..."
	find . -name "*.o" -type f -delete
	find . -type f -executable -delete

# Help target
help:
	@echo "Assembly Examples Makefile"
	@echo "--------------------------"
	@echo "Available targets:"
	@echo "  all       - Build all examples"
	@echo "  basics    - Build basic assembly examples"
	@echo "  data      - Build data manipulation examples"
	@echo "  control   - Build control flow examples"
	@echo "  functions - Build function examples"
	@echo "  io        - Build I/O examples"
	@echo "  advanced  - Build advanced examples"
	@echo "  clean     - Remove all generated files"
	@echo "  help      - Display this help message"
	@echo
	@echo "Example usage:"
	@echo "  make io   - Build only the I/O examples"
	@echo "  make all  - Build all examples"
	@echo "  make clean - Clean up all generated files"

.PHONY: all basics data control functions io advanced clean help io_functions