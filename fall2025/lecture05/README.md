# Lecture 5 - Introduction to x86-64 Assembly

This directory contains introductory x86-64 assembly examples for Fall 2025 Lecture 5.

## Contents

### C Source Files (for comparison)
- **hello.c** - Simple "Hello world" program in C
- **hellocommented.c** - Detailed commented version explaining basic C structure
- **adding.c** - Simple addition function in C
- **addingcommented.c** - Detailed commented version explaining function calls

### Assembly Files - Basic Programs
- **smallest.s** - Minimal assembly program (exit with code)
- **smallestcommented.s** - Detailed explanation of minimal assembly structure
- **smaller.s** - Simple assembly program with basic operations
- **smallercommented.s** - Detailed explanation of basic assembly operations

### Assembly Files - Generated from C
- **adding.s** - Assembly code generated from adding.c (using `gcc -S`)
- **addingcommented.s** - Detailed explanation of function call conventions

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific C programs:
```bash
make hello
make adding
```

### Compile assembly programs:
```bash
make smallest
make smaller
```

Assembly files are assembled and linked using:
```bash
as smallest.s -o smallest.o
ld smallest.o -o smallest
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Minimal Assembly Program
```bash
./smallest
echo $?  # Check exit code (should show 255, which is -1 as unsigned byte)
```

**Expected behavior:**
- Program exits immediately with exit code -1 (displayed as 255)
- Demonstrates direct system call interface

### Smaller Assembly Program
```bash
./smaller
echo $?  # Check exit code
```

**Expected behavior:**
- Program performs basic register operations
- Exits with specific exit code

### C Programs for Comparison
```bash
./hello
# Output: Hello world

./adding
# Output: 30 (result of 10 + 20)
```

### Generating Assembly from C
You can generate assembly code from any C file:
```bash
gcc -S hello.c -o hello_new.s
gcc -S adding.c -o adding_new.s
```

This allows you to see how the C compiler translates high-level code into assembly.

## Key Concepts Demonstrated

### Assembly Language Basics
- **Registers** - CPU scratchpad memory (rax, rdi, etc.)
- **System calls** - Direct interface to operating system (exit syscall #60)
- **Sections** - `.text` for code, `.data` for initialized data
- **Labels** - `_start` is the entry point for programs linked with `ld`

### AT&T Syntax
- Register names start with `%` (e.g., `%rax`, `%rdi`)
- Immediate values start with `$` (e.g., `$60`, `$-1`)
- Source comes before destination in operations: `mov $60, %rax`

### Exit Codes
- Exit codes are stored in `%rdi` register
- Exit syscall number (60) goes in `%rax`
- Exit codes are single unsigned bytes (0-255)
- Negative values wrap around (e.g., -1 becomes 255)

### Compilation Pipeline
1. **Compiler** - Translates C to assembly (`gcc -S`)
2. **Assembler** - Translates assembly to machine code (`as`)
3. **Linker** - Creates executable from object files (`ld`)

### Function Calls
- Calling conventions determine how arguments are passed
- Return values typically use `%rax` register
- Stack is used for local variables and return addresses

## Notes

- All assembly examples use AT&T syntax (GNU assembler format)
- x86-64 has 16 general-purpose 64-bit registers
- Only 128 bytes of register memory, but extremely fast access
- Programs linked with `ld` use `_start` as entry point
- Programs using C library functions use `main` as entry point
- Commented versions provide line-by-line explanations of each instruction
- Compare C source with generated assembly to understand compilation

## Resources

- Read chapters 6 and 7 of "Dive into Systems" for detailed assembly coverage
- Chapter 6: Introduction to assembly concepts
- Chapter 7: Detailed x86-64 assembly reference
