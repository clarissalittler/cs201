# Spring 2025 - CS201 Systems Programming

This directory contains topic-based materials for Spring 2025, organized by concept rather than by lecture number.

## Directory Structure

Spring 2025 uses a **topic-based organization** instead of sequential lectures:

### Core Topic Directories
- `assembly/` - Assembly language programming examples
- `forking/` - Process creation and management
- `threading/` - Thread programming and concurrency
- `in-class/` - Examples worked through during class sessions

### Video Lecture Materials
- `video1/` - First video lecture materials
- `video2/` - Second video lecture materials
- `video3/` - Third video lecture materials

## Topic-Based vs Lecture-Based

### Advantages of Topic Organization

Instead of following lectures 1-13, Spring 2025 groups related concepts together:

- **Easier to find specific topics** - Want to learn about threading? Go to `threading/`
- **Focused study** - All related examples in one place
- **Better for review** - Quickly access all assembly or forking examples
- **Self-paced learning** - Study topics in any order you prefer

### Recommended Learning Order

While you can explore topics independently, here's a suggested sequence:

1. **video1/** - Introduction and fundamentals
2. **video2/** - Building on basics
3. **video3/** - Advanced concepts
4. **assembly/** - Low-level programming
5. **forking/** - Process management
6. **threading/** - Concurrent programming
7. **in-class/** - Practical applications

## Topic Details

### assembly/

**Focus:** x86-64 assembly language programming

Contains:
- Basic assembly programs
- Assembly syntax examples
- Register usage demonstrations
- Control flow in assembly
- Function calls and stack operations

**Building:**
```bash
cd assembly
gcc filename.s -o filename
./filename
```

**See also:** `/assemblyGuide.org` and `/assembly-examples/` for comprehensive assembly materials

### forking/

**Focus:** Process creation and management

Topics covered:
- `fork()` - Creating child processes
- `exec()` - Executing programs
- `wait()` - Process synchronization
- Process hierarchies
- Pipes for communication

**Building:**
```bash
cd forking
gcc -Wall filename.c -o filename
./filename
```

**See also:** `/concurrency-tutorial/fork-examples/` for more process examples

### threading/

**Focus:** Multi-threaded programming with pthreads

Topics covered:
- Thread creation with `pthread_create()`
- Thread joining with `pthread_join()`
- Mutexes for synchronization
- Condition variables
- Race conditions
- Thread-safe programming

**Building:**
```bash
cd threading
gcc -Wall -pthread filename.c -o filename
./filename
```

**See also:** `/concurrency-tutorial/thread-examples/` for comprehensive threading materials

### in-class/

**Focus:** Examples demonstrated during class sessions

Contains:
- Live coding examples
- Problem-solving demonstrations
- Student questions addressed in code
- Practical applications of concepts

**Nature:** Code may be more experimental or exploratory than polished examples

### video1/, video2/, video3/

**Focus:** Materials accompanying video lectures

Each contains:
- **Agenda.org** - Topics covered in the video
- Source code examples from the lecture
- Demonstrations shown in the video

**Recommended:** Watch corresponding videos while studying these materials

## Building Examples

### General C Programs

```bash
# Basic compilation
gcc -Wall filename.c -o filename
./filename

# With debugging
gcc -Wall -g filename.c -o filename
gdb ./filename
```

### Threaded Programs

```bash
# Always use -pthread for thread programs
gcc -Wall -pthread threads.c -o threads
./threads
```

### Assembly Programs

```bash
# Using gcc (recommended)
gcc filename.s -o filename
./filename

# Manual assembly and linking
as filename.s -o filename.o
ld filename.o -o filename
./filename
```

## File Organization

Each topic directory contains:
- Source files (`.c` for C, `.s` for assembly)
- Some may have commented versions (`filenamecommented.c`)
- Agenda files where applicable (`Agenda.org`)

## Getting Started

### For New Students

**Option 1: Sequential Video Approach**
1. Start with video1/ and watch the corresponding lecture
2. Compile and run examples from video1/
3. Progress through video2/ and video3/
4. Dive deeper into topic directories as needed

**Option 2: Topic-Based Approach**
1. Identify topics you want to learn
2. Go directly to that topic directory
3. Work through examples in order
4. Cross-reference with other materials

**Option 3: Hands-On Approach**
1. Browse in-class/ for practical examples
2. Compile and run code
3. Refer to topic directories for theory
4. Watch videos for detailed explanations

### For Specific Goals

**Learning Assembly:**
- Start with `assembly/`
- Reference `/assemblyGuide.org`
- Explore `/assembly-examples/` for structured learning

**Understanding Processes:**
- Study `forking/`
- See `/concurrency-tutorial/fork-examples/`
- Practice with `in-class/` examples

**Mastering Threading:**
- Begin with `threading/`
- Read `/concurrency-tutorial/thread-examples/`
- Work through synchronization examples
- Review `/deadlock-lecture/` for pitfalls

## Comparison with Other Semesters

### Spring 2025 vs Fall 2024/2025/Winter 2025

| Aspect | Spring 2025 | Other Semesters |
|--------|-------------|-----------------|
| Organization | Topic-based | Lecture-based (1-13) |
| Navigation | By concept | By sequence |
| Best for | Targeted learning | Sequential course |
| Video materials | Integrated | Separate |
| In-class examples | Dedicated directory | Mixed with lectures |

**Choose based on your learning style:**
- **Sequential learners** → Fall or Winter materials
- **Topic-focused learners** → Spring 2025
- **Video learners** → Spring 2025 with videos
- **Comprehensive study** → Use multiple semesters

## Key Topics Covered

### Assembly Language
- AT&T syntax
- Registers and memory
- Control structures
- Functions and stack
- System calls

### Process Management
- Process creation (`fork()`)
- Program execution (`exec()`)
- Parent-child relationships
- Process synchronization (`wait()`)
- Inter-process communication (pipes)

### Thread Programming
- Thread lifecycle
- Concurrent execution
- Shared memory
- Synchronization (mutexes, condition variables)
- Common pitfalls (race conditions, deadlocks)

## Additional Resources

### In This Repository

- `/README.md` - Main navigation guide
- `/assemblyGuide.org` - Complete assembly reference
- `/concurrency-tutorial/` - Comprehensive concurrency guide
- `/assembly-examples/` - Organized assembly collection
- `/IPC_TUTORIAL.md` - Inter-process communication guide

### Related Directories

- `/fall2024/` - Fall 2024 lecture materials
- `/fall2025/` - Fall 2025 lecture materials
- `/winter2025/` - Winter 2025 lecture materials
- `/file-intro/` - File I/O operations
- `/socket-intro/` - Network programming
- `/deadlock-lecture/` - Deadlock examples

## Learning Strategies

### Depth-First Approach
Pick one topic and master it completely:
1. Read all examples in the topic directory
2. Compile and run each one
3. Modify code and experiment
4. Consult external references
5. Move to next topic

### Breadth-First Approach
Get overview of all topics:
1. Sample one example from each directory
2. Understand high-level concepts
3. Return for deeper study as needed
4. Connect concepts across topics

### Project-Based Approach
Build something using multiple topics:
1. Design a multi-process application
2. Use forking/ for process creation
3. Use threading/ for concurrency
4. Apply assembly/ for optimization
5. Integrate learnings from in-class/

## Troubleshooting

### Can't find a specific concept
- Check topic directories first
- Browse in-class/ for practical examples
- Search video materials (Agenda.org files)
- Consult main repository README for additional locations

### Compilation issues
- Ensure proper flags (`-pthread` for threads)
- Check AT&T vs Intel syntax for assembly
- Verify include statements
- Read error messages carefully

### Understanding flow between topics
- Topics build on each other
- Forking is simpler than threading
- Assembly helps understand low-level operations
- in-class/ shows integration

## Development Tools

```bash
# Compiler
gcc --version

# Debugger
gdb program

# Memory checker
valgrind ./program

# Thread analyzer
valgrind --tool=helgrind ./threaded_program

# Assembly viewer
objdump -d program
```

## Notes

- Organization optimized for topic-based learning
- All assembly uses AT&T syntax
- Video materials complement directory contents
- in-class/ examples may be more exploratory
- Agenda files use `Agenda.org` (capital A)
- Cross-reference with other semesters for different perspectives

## See Also

- [Main README](../README.md) - Complete repository overview
- [Assembly Guide](../assemblyGuide.org) - Assembly programming reference
- [Concurrency Tutorial](../concurrency-tutorial/README.md) - Detailed concurrency guide
- [Fall 2024](../fall2024/README.md) - Sequential lecture approach
- [Winter 2025](../winter2025/README.md) - Sequential with exemplary documentation
