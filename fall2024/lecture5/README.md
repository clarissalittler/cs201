# Lecture 5 - Introduction to x86-64 Assembly

This directory contains introductory x86-64 assembly programs demonstrating the basics of assembly language syntax and structure.

## Contents

This directory features progressively simpler assembly programs, each demonstrating fundamental assembly concepts:

### Assembly Source Files

- **simplest.s** - The absolute minimal assembly program (just an exit syscall)
- **simplestcommented.s** - Commented version explaining the basic program structure
- **simpler.s** - Slightly more complex assembly program
- **simplercommented.s** - Commented version with detailed explanations
- **simple.s** - A simple assembly program demonstrating basic operations
- **simplecommented.s** - Fully commented version with educational annotations

## Compilation

### Compile all examples:
```bash
make
```

### Compile a specific example:
```bash
make simplest
make simpler
make simple
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Simplest Program
```bash
./simplest
echo $?
```

**Expected behavior:**
- The program exits immediately with a return code
- Use `echo $?` to see the exit status in your shell

### Simpler Program
```bash
./simpler
echo $?
```

**Expected behavior:**
- Program performs basic register operations and exits
- Check exit status with `echo $?`

### Simple Program
```bash
./simple
echo $?
```

**Expected behavior:**
- Demonstrates more assembly operations before exiting
- Exit status shows the result

## Key Concepts Demonstrated

### Program Structure
- `.section .text` - Defines the code section
- `.global _start` - Declares the entry point symbol
- `_start:` label - The actual entry point where execution begins

### Basic Instructions
- `mov` - Move data between registers and immediate values
- Register usage (e.g., `%rax`, `%rdi`)
- Immediate values (e.g., `$60`)

### System Calls
- `syscall` instruction - Invokes kernel services
- Exit syscall (syscall number 60 in `%rax`)
- Passing arguments in registers (exit code in `%rdi`)

### Assembly Fundamentals
- Comments using `#`
- Hexadecimal and decimal values
- AT&T syntax (source, destination ordering)

## Notes

- All programs use x86-64 (64-bit) assembly
- Programs follow the Linux x86-64 calling convention
- AT&T syntax is used (as opposed to Intel syntax)
- System call numbers are specific to Linux
- The `$` prefix indicates immediate values
- The `%` prefix indicates registers
- These are minimal programs focused on learning assembly basics
- Reading the commented versions is highly recommended for understanding each instruction
