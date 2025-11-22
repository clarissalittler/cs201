# Winter 2025 - CS201 Systems Programming

This directory contains lecture materials for Winter 2025 (lectures 1-13 plus lecture-inf), covering C programming, x86-64 assembly, and operating systems concepts.

## Directory Structure

### Lecture Materials
- `lecture1/` through `lecture13/` - Sequential lecture materials
- `lecture-inf/` - Additional supplementary material
- `finalproj/` - Final project scaffolds and resources

## Lecture Organization

Winter 2025 materials follow a sequential structure with all lectures numbered 1-13 (no missing numbers). Each lecture directory contains:

- **Agenda.org** - Lecture outline and learning objectives
- **Source code** - `.c` and `.s` files demonstrating concepts
- **Commented versions** - Files with `commented` suffix for detailed explanations

## Highlighted Lecture

### lecture13/ - Exemplary Documentation ⭐

The `lecture13/` directory serves as a **model** for how lecture materials should be organized. It includes:
- Comprehensive README with clear instructions
- Well-organized example code
- Detailed running instructions for client-server examples
- Troubleshooting section
- Expected behavior descriptions

**Use lecture13 as a template** when creating your own examples or studying IPC concepts.

## Topics Covered

### Early Lectures (1-5): C Fundamentals
- Introduction to systems programming
- C syntax, data types, operators
- Control structures
- Functions and scope
- Pointers and memory

### Middle Lectures (6-9): Advanced C and Assembly
- Arrays and strings
- Structs and dynamic memory
- Introduction to x86-64 assembly
- Assembly programming fundamentals

### Late Lectures (10-13): Concurrency and IPC
- Process creation and management
- Threading and synchronization
- Inter-process communication
- Advanced systems topics

## Building Examples

### General Compilation

Most C programs can be compiled with:

```bash
gcc -Wall filename.c -o filename
./filename
```

### Threading Programs

Programs using pthreads:

```bash
gcc -Wall -pthread threads.c -o threads
./threads
```

### Assembly Programs

Using GCC for easy compilation:

```bash
gcc filename.s -o filename
./filename
```

### Lecture 13 Examples

lecture13 has a comprehensive Makefile:

```bash
cd lecture13
make              # Build all examples
make clean        # Remove compiled files

# Or build specific examples:
make task_distributor worker
make sharedMemprod sharedMemcons
make chat_server chat_client
make fileserver fileclient
```

## Getting Started

### For Beginners

1. **Start with lecture1** - Introduction to the course and C
2. **Work sequentially** through lectures 1-13
3. **Read Agenda.org** in each lecture for topics
4. **Compile and run** every example
5. **Study lecture13** for best practices in documentation

### For Specific Topics

**C Programming:**
- Lectures 1-7

**Assembly Programming:**
- Lectures 8-9
- Also see `/assemblyGuide.org` and `/assembly-examples/`

**Process Management:**
- Lectures 10-11

**Threading and Synchronization:**
- Lectures 11-12

**Inter-Process Communication:**
- Lecture 13 ⭐ (excellent examples and documentation)
- Also see `/IPC_TUTORIAL.md` and `/ipc-lecture/`

## File Naming Convention

```
example.c          - Original source
examplecommented.c - Detailed annotations
example.s          - Assembly source
examplecommented.s - Annotated assembly
```

**Learning strategy:** Read commented version first, then compare with original.

## Special Directories

### finalproj/

Contains scaffolding for the final project:
- `server_scaffold.c` - Template server implementation
- `server_scaffoldcommented.c` - Annotated version
- Intentional TODOs for student completion

**Note:** These are intentional incomplete examples for students to finish.

### lecture-inf/

Supplementary materials and additional examples beyond the regular lectures.

## Comparison with Other Semesters

### Winter 2025 vs Fall 2024/2025

**Similarities:**
- Covers same core topics
- Uses same AT&T assembly syntax
- Focuses on C and systems programming

**Differences:**
- **Complete sequence**: lectures 1-13 with no gaps
- **Different pacing**: Some topics spread across different lectures
- **Best documentation**: lecture13 is exemplary
- **Final project**: Includes scaffolding in finalproj/
- **Naming**: Uses `Agenda.org` (capital A) consistently

## Key Concepts by Topic

### C Programming
- Variables and types
- Pointers and arrays
- Memory management
- Structs and unions
- Function pointers

### Assembly Language
- AT&T syntax
- Register usage
- Memory addressing
- Function calls
- System calls

### Process Management
- `fork()` - Creating processes
- `exec()` - Executing programs
- `wait()` - Process coordination
- Process hierarchy

### Threading
- `pthread_create()` - Thread creation
- `pthread_join()` - Thread synchronization
- Mutexes and condition variables
- Race conditions and deadlocks

### Inter-Process Communication (IPC)
- **Message Queues** - Structured messaging between processes
- **Shared Memory** - High-speed data sharing
- **Named Pipes (FIFOs)** - Bidirectional communication
- **Sockets** - Network and local communication
- **Semaphores** - Synchronization primitives

## Learning Resources

### In This Repository

- `/README.md` - Main repository guide
- `/assemblyGuide.org` - Assembly programming reference
- `/concurrency-tutorial/` - Comprehensive concurrency guide
- `/IPC_TUTORIAL.md` - Detailed IPC mechanisms
- `/assembly-examples/` - Organized assembly examples

### Other Semesters

- `/fall2024/` - Fall 2024 materials
- `/fall2025/` - Fall 2025 materials
- `/spring2025/` - Topic-based organization

## Compilation Tips

### Useful GCC Flags

```bash
-Wall              # All warnings
-Wextra            # Extra warnings
-g                 # Debug symbols
-O2                # Optimization level 2
-pthread           # Thread support
-lm                # Math library
-lrt               # Real-time library (POSIX IPC)
```

### Example Commands

```bash
# Basic C program
gcc -Wall -g hello.c -o hello

# With threading
gcc -Wall -g -pthread threads.c -o threads

# With POSIX IPC
gcc -Wall -g -pthread -lrt ipc_program.c -o ipc_program

# Assembly program
gcc -Wall -g assembly.s -o assembly
```

## Running Multi-Process Examples

Many examples in lecture13 require multiple terminals:

### Example: Message Queue

**Terminal 1 (Receiver):**
```bash
cd lecture13
./msgqReceiver
```

**Terminal 2 (Sender):**
```bash
cd lecture13
./msgqSender
```

### Example: Shared Memory

**Terminal 1 (Producer):**
```bash
./sharedMemprod
```

**Terminal 2 (Consumer):**
```bash
./sharedMemcons
```

**See lecture13/README.md for complete instructions.**

## Troubleshooting

### IPC Resources Already Exist

```bash
# List message queues
ipcs -q

# Remove specific message queue
ipcrm -q <msqid>

# List shared memory
ipcs -m

# Remove specific shared memory
ipcrm -m <shmid>

# Remove named pipes
rm fifo_server fifo_client
```

### Compilation Errors

- Ensure `-pthread` flag for threaded programs
- Ensure `-lrt` flag for POSIX IPC (message queues, shared memory)
- Check that all headers are included
- Verify syntax (especially for assembly)

### Permission Issues

```bash
# Make executable
chmod +x program

# Run
./program
```

### Port Already in Use (Socket Examples)

- Wait 30-60 seconds for port to be released
- Use a different port number
- Add `SO_REUSEADDR` option (see socket examples)

## Best Practices for Learning

1. **Read Agenda.org first** - Understand lecture goals
2. **Compile every example** - Verify it works
3. **Run and observe** - See actual behavior
4. **Read commented versions** - Understand details
5. **Modify and experiment** - Learn by doing
6. **Use debugging tools** - gdb, valgrind
7. **Study lecture13** - Model of good documentation

## Development Environment

### Linux/Unix
All examples designed for Linux/Unix environments.

### Online Alternatives
- https://www.onlinegdb.com/ - Online C compiler
- https://godbolt.org/ - Compiler Explorer (great for assembly)

### Course Server
If available: cslinux.pcc.edu

## Final Project

The `finalproj/` directory contains:
- Server implementation scaffolds
- Commented versions with explanations
- TODOs for student completion

**This is intentional:** Learn by implementing the missing pieces.

## Notes

- All agenda files use `Agenda.org` (capital A)
- All assembly uses AT&T syntax (not Intel)
- lecture13 is the gold standard for documentation
- Some examples require root or specific permissions
- Programs assume POSIX-compliant system

## See Also

- [Main README](../README.md) - Repository overview
- [lecture13 README](lecture13/README.md) - Exemplary lecture documentation ⭐
- [Concurrency Tutorial](../concurrency-tutorial/README.md) - Detailed concurrency guide
- [IPC Tutorial](../IPC_TUTORIAL.md) - Comprehensive IPC guide
- [Assembly Guide](../assemblyGuide.org) - Assembly reference
