# Fall 2024 - CS201 Systems Programming

This directory contains lecture materials from Fall 2024 (lectures 1-11), covering foundational topics in C programming, x86-64 assembly, and systems programming.

## Directory Structure

This semester's materials are organized by lecture number:
- `lecture1/` through `lecture11/` - Sequential lecture materials
- `in-class/` - Additional in-class examples and demonstrations

## Lecture Topics Overview

### Early Lectures (1-4): C Programming Fundamentals
- **lecture1/** - Introduction to C, basic syntax, compilation
- **lecture2/** - Variables, data types, operators
- **lecture3/** - Control flow (if, loops, switch)
- **lecture4/** - Pointers and arrays

### Mid Lectures (5-8): Assembly and Low-Level Programming
- **lecture5/** - Introduction to x86-64 assembly (has Makefile ✓)
- **lecture6/** - Assembly control flow
- **lecture7/** - Assembly functions and stack (has Makefile ✓)
- **lecture8/** - Advanced assembly, I/O, AVX-512 (has Makefile ✓)

### Late Lectures (9-11): Processes and Concurrency
- **lecture9/** - Process creation (fork, exec), pipes
- **lecture10/** - Threading basics
- **lecture11/** - Signals and process control

## Building Examples

### Lectures with Makefiles (5, 7, 8)

These lectures include Makefiles for easy compilation:

```bash
cd lecture5
make           # Build all examples
make clean     # Remove compiled files
```

### Lectures without Makefiles (1, 2, 3, 4, 6, 9, 10, 11)

Compile C programs manually:

```bash
# Basic C program
gcc filename.c -o filename

# Program using math library
gcc filename.c -o filename -lm

# Program using threads
gcc filename.c -o filename -lpthread

# Run
./filename
```

For assembly programs:

```bash
# Using gcc (recommended)
gcc filename.s -o filename

# Or manually with as and ld
as filename.s -o filename.o
ld filename.o -o filename

# Run
./filename
```

## File Naming Convention

Throughout this directory you'll find two versions of many files:
- **Original files**: `filename.c` or `filename.s` - Core examples
- **Commented files**: `filenamecommented.c` or `filenamecommented.s` - Detailed explanations

**Learning tip:** Start with the commented version to understand the concepts, then study the original for cleaner code.

## Getting Started

### For Absolute Beginners
1. Start with `lecture1/` for basic C programming
2. Progress sequentially through lectures 1-4
3. Work through examples both reading and compiling
4. Compare original and commented versions

### For Assembly Topics
1. Start with `lecture5/` for x86-64 introduction
2. Reference `/assemblyGuide.org` in the repository root
3. Study both .c and .s versions to see C-to-assembly translation
4. Progress through lectures 5-8 for advanced topics

### For Concurrency Topics
1. Start with `lecture9/` for process basics
2. Continue to `lecture10/` for threading
3. For deeper study, see `/concurrency-tutorial/` in repository root
4. Complete with `lecture11/` for signals

## Common Compilation Patterns

### C Programs

```bash
# Simple program
gcc hello.c -o hello

# With warnings enabled (recommended)
gcc -Wall hello.c -o hello

# With debugging symbols
gcc -g -Wall hello.c -o hello

# Multiple source files
gcc main.c helper.c -o program

# With threading
gcc -pthread threads.c -o threads
```

### Assembly Programs

```bash
# AT&T syntax (used in this course)
gcc assembly.s -o assembly

# With debugging
gcc -g assembly.s -o assembly

# Link with C libraries
gcc assembly.s library.c -o program
```

## Key Topics Covered

### C Programming (Lectures 1-4)
- Basic syntax and data types
- Pointers and memory addresses
- Arrays and strings
- Control structures
- Function definitions
- Header files and compilation

### Bit Manipulation
- Binary and hexadecimal representation
- Bitwise operators (&, |, ^, ~, <<, >>)
- Two's complement arithmetic
- Bit masking and manipulation

### x86-64 Assembly (Lectures 5-8)
- AT&T syntax
- Register usage
- Memory addressing modes
- Control flow (jumps, conditionals, loops)
- Function calls and stack frames
- System calls
- AVX-512 vector instructions
- Floating-point operations

### Process Management (Lectures 9-11)
- `fork()` - Process creation
- `exec()` family - Program execution
- `wait()` - Process synchronization
- Pipes - Inter-process communication
- `pthread` - Threading basics
- Signals - Asynchronous events

## Recommended Learning Path

1. **Sequential approach** - Work through lectures 1-11 in order
2. **Topic-based approach** - Jump to specific topics:
   - C programming: Lectures 1-4
   - Assembly: Lectures 5-8
   - Concurrency: Lectures 9-11
3. **Hands-on approach** - Compile and run every example
4. **Deep-dive approach** - Read commented versions, modify code, experiment

## Additional Resources

### In Repository
- `/README.md` - Main repository guide
- `/assemblyGuide.org` - Comprehensive assembly reference
- `/concurrency-tutorial/` - Detailed concurrency examples and exercises
- `/assembly-examples/` - Organized assembly example collection

### Semester-Specific
- `/winter2025/` - Alternative lecture organization
- `/spring2025/` - Topic-based organization
- `/fall2025/` - Updated lecture materials

## Troubleshooting

### Compilation errors
- Ensure gcc is installed: `gcc --version`
- Check for typos in filenames
- For assembly, ensure you're using AT&T syntax
- Use `-Wall` to see all warnings

### Missing libraries
```bash
# Threading library
gcc -pthread program.c -o program

# Math library
gcc -lm program.c -o program

# Both
gcc -pthread -lm program.c -o program
```

### Permission denied when running
```bash
chmod +x program  # Make executable
./program         # Run
```

### Assembly linking errors
- Use `gcc` instead of `as` + `ld` for easier linking
- Ensure main function is properly defined
- Check for undefined symbols

## Notes

- This is legacy material from Fall 2024
- Some lectures may reference course-specific resources (e.g., cslinux.pcc.edu)
- Examples assume a Linux/Unix environment
- Code style may vary between lectures as teaching style evolved
- All assembly uses AT&T syntax (not Intel syntax)

## See Also

- [Main repository README](../README.md) for complete overview
- [Concurrency Tutorial](../concurrency-tutorial/) for deeper concurrency topics
- [Assembly Examples](../assembly-examples/) for organized assembly collection
- [Fall 2025](../fall2025/) for updated materials
