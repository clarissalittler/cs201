# Fall 2025 - CS201 Systems Programming

This directory contains current lecture materials for Fall 2025, covering C programming, x86-64 assembly, and systems programming concepts.

## Directory Structure

### Lecture Materials
- `lecture01/` - Introduction to C and systems programming
- `lecture02/` - C fundamentals and data types
- `lecture03/` - Control flow and functions
- `lecture05/` - Assembly basics (lecture 04 was a blackboard lecture)
- `lecture06/` - Pointers and memory
- `lecture07/` - Advanced assembly topics
- `lecture08/` - More assembly concepts
- `lecture09/` - Process creation and management
- `lecture11/` - Advanced concurrency topics (lecture 10 was a blackboard lecture)
- `lecture13/` - Inter-process communication (lecture 12 was a blackboard lecture)
- `lecture14/` - Advanced topics

### Topic-Specific Directories
- `file-lecture/` - File I/O operations in C
- `pointerExtras/` - Additional pointer examples and exercises

## Note About Missing Lectures

**Lectures 04, 10, and 12 are not in this directory** because they were blackboard/whiteboard lectures with no code examples. This is intentional and not an error.

## File Organization

Each lecture directory contains:
- **Agenda.org** - Lecture outline and topics covered
- **Source files** - `.c` files for C examples, `.s` files for assembly
- **Commented versions** - Files ending in `commented.c` or `commented.s` with detailed explanations

### Naming Convention

- `example.c` - Original C source file
- `examplecommented.c` - Same code with detailed comments
- `example.s` - Assembly source file
- `examplecommented.s` - Assembly with line-by-line explanations

**Learning tip:** Read the commented version first to understand concepts, then study the original for clean code patterns.

## Building Examples

### Compiling C Programs

```bash
# Basic compilation
gcc filename.c -o filename

# With all warnings (recommended for learning)
gcc -Wall filename.c -o filename

# With debugging symbols
gcc -g -Wall filename.c -o filename

# Run the program
./filename
```

### Compiling Assembly Programs

```bash
# Using gcc (easiest method)
gcc filename.s -o filename

# Or using as and ld
as filename.s -o filename.o
ld filename.o -o filename

# Run
./filename
```

### Threading Programs

Programs using pthreads require the `-pthread` flag:

```bash
gcc -pthread threads.c -o threads
./threads
```

## Getting Started

### For New Students

1. **Start with lecture01** - Basic C programming concepts
2. **Work sequentially** through lectures in numerical order
3. **Read Agenda.org** in each lecture to see topics covered
4. **Compile and run** every example to see it in action
5. **Study commented files** for detailed explanations

### For Specific Topics

**C Programming Basics:**
- lecture01, lecture02, lecture03

**Pointers and Memory:**
- lecture06, pointerExtras/

**File Operations:**
- file-lecture/

**Assembly Programming:**
- lecture05, lecture07, lecture08
- Also see `/assembly-examples/` and `/assemblyGuide.org` in repository root

**Processes and Concurrency:**
- lecture09, lecture11
- Also see `/concurrency-tutorial/` for comprehensive examples

**Inter-Process Communication:**
- lecture13
- Also see `/ipc-lecture/` and `/IPC_TUTORIAL.md`

## Key Topics Covered

### C Programming Fundamentals
- Variables, data types, operators
- Control structures (if, while, for, switch)
- Functions and scope
- Pointers and memory addresses
- Arrays and strings
- Structs and type definitions

### Systems Programming Concepts
- Memory layout and management
- Pointers and indirection
- Bit manipulation
- Low-level I/O operations
- System calls

### x86-64 Assembly Language
- AT&T syntax (used throughout)
- Register usage and calling conventions
- Memory addressing modes
- Control flow implementation
- Function calls and stack frames
- System calls in assembly

### Process Management
- Process creation with `fork()`
- Program execution with `exec()` family
- Process communication with pipes
- Process coordination with `wait()`

### Concurrency
- Threading with pthreads
- Synchronization primitives
- Race conditions and deadlocks
- Thread-safe programming

### Inter-Process Communication
- Message queues
- Shared memory
- Named pipes (FIFOs)
- Sockets
- Semaphores

## Compilation Tips

### Common Flags

```bash
-Wall              # Enable all warnings
-g                 # Include debugging information
-O2                # Optimize for performance
-pthread           # Link pthread library
-lm                # Link math library
-std=c11           # Use C11 standard
```

### Example Compilation Commands

```bash
# Simple program
gcc -Wall hello.c -o hello

# Program with math
gcc -Wall -lm calculate.c -o calculate

# Threaded program
gcc -Wall -pthread threads.c -o threads

# With debugging
gcc -Wall -g -pthread debug_me.c -o debug_me
```

## Exploring the Code

### Reading Order for Each Lecture

1. **Read Agenda.org** - Understand what topics are covered
2. **List the files** - See what examples are available
   ```bash
   ls lecture01/
   ```
3. **Read commented version** - Study the detailed explanations
4. **Compare with original** - See clean code without comments
5. **Compile and run** - Verify your understanding
6. **Experiment** - Modify the code and recompile

### Example Workflow

```bash
# Navigate to lecture
cd lecture01

# See what's there
ls

# Read the agenda
cat Agenda.org

# Compile an example
gcc -Wall bitmunger1.c -o bitmunger1

# Run it
./bitmunger1

# Read the commented version
less bitmunger1commented.c
```

## Additional Resources

### In This Repository
- `/README.md` - Main repository overview and navigation
- `/assemblyGuide.org` - Comprehensive assembly programming guide
- `/concurrency-tutorial/` - In-depth concurrency examples and exercises
- `/assembly-examples/` - Organized collection of assembly examples
- `/file-intro/` - More file operation examples
- `/socket-intro/` - Network programming basics
- `/deadlock-lecture/` - Deadlock examples and prevention

### Other Semesters
- `/fall2024/` - Previous semester's materials
- `/winter2025/` - Alternative organization of topics
- `/spring2025/` - Topic-based organization

## Troubleshooting

### "command not found" when running program
```bash
# Make sure you use ./
./program        # Correct
program          # Wrong (unless in PATH)
```

### "Permission denied"
```bash
# Make file executable
chmod +x program
./program
```

### Compilation errors
- Check for typos in filenames
- Ensure gcc is installed: `gcc --version`
- Read error messages carefully - they often indicate the exact problem
- For assembly, verify you're using AT&T syntax (not Intel)

### Assembly linking errors
- Use `gcc filename.s` instead of manually using `as` and `ld`
- Ensures proper library linking and startup code

## Best Practices for Learning

1. **Type code yourself** - Don't just copy-paste
2. **Experiment** - Change values, break things, fix them
3. **Use debugging tools** - gdb, valgrind
4. **Read error messages** - They're educational!
5. **Compare versions** - Study differences between original and commented
6. **Ask questions** - Use office hours, discussion boards

## Development Environment

### Recommended Tools

```bash
# Compiler
gcc --version

# Debugger
gdb --version

# Memory checker
valgrind --version

# Text editor options
vim, emacs, nano, vscode, etc.
```

### Running Code on cslinux.pcc.edu

If you have access to the course server:
```bash
ssh username@cslinux.pcc.edu
# Navigate to your directory
# Compile and run as normal
```

Alternatively, use online C compilers for quick tests:
- https://www.onlinegdb.com/
- https://godbolt.org/ (great for seeing assembly output)

## Notes

- All assembly code uses AT&T syntax (not Intel syntax)
- Each lecture's Agenda.org file contains the lecture plan and topics
- Some code examples build on previous lectures
- Programs assume a Linux/Unix environment (may need modifications for Windows)
- Missing lecture numbers (04, 10, 12) were blackboard lectures

## See Also

- [Main README](../README.md) - Complete repository guide
- [Concurrency Tutorial](../concurrency-tutorial/README.md) - Comprehensive concurrency guide
- [Assembly Examples](../assembly-examples/README.md) - Structured assembly learning path
- [IPC Tutorial](../IPC_TUTORIAL.md) - Detailed IPC mechanisms guide
