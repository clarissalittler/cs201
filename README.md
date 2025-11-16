# CS201 - Systems Programming

Welcome to CS201! This repository contains lecture materials, code examples, and tutorials for learning systems programming with C and x86-64 assembly.

## Repository Organization

This repository is organized by semester and topic. Here's a guide to help you navigate:

### Semester-Based Materials

#### `fall2024/`
Legacy lecture materials from Fall 2024 (lectures 1-11). Contains foundational examples covering:
- Basic C programming and syntax
- Bit manipulation and number representations
- Pointers and memory
- Assembly language basics
- Control flow in assembly
- Advanced assembly topics (functions, I/O, AVX-512)
- Process creation and management (fork, exec, pipes)
- Threading basics
- Signals

#### `fall2025/`
Current Fall 2025 lecture materials (lectures 01, 02, 03, 05, 06, 07, 08, 09, 11, 13, 14). Each lecture folder contains:
- `Agenda.org` - Lecture outline and topics
- Source code examples demonstrating concepts covered in that lecture
- Both original and commented versions of code for learning

**Note**: Some lecture numbers are missing (04, 10, 12) as content may have been reorganized or combined with other lectures.

#### `winter2025/`
Winter 2025 lecture materials (lectures 1-13 plus lecture-inf). Covers similar foundational topics with different pacing and organization. Each folder includes:
- `agenda.org` - Lecture plans and topics
- Code examples and demonstrations
- Additional resources and guides

#### `spring2025/`
Topic-based organization focused on specific concepts:
- `assembly/` - Assembly language programming
- `forking/` - Process creation and management
- `threading/` - Thread programming and synchronization
- `video1/`, `video2/`, `video3/` - Video lecture materials
- `in-class/` - In-class examples and demonstrations

### Tutorial Collections

#### `concurrency-tutorial/`
Comprehensive guide to concurrent programming in C. **Start here for concurrency topics!**

Structure:
- **`concurrencyGuide.org`** - Theoretical guide to concurrency concepts
- **`EXAMPLE_INDEX.md`** - Catalog of all examples with descriptions
- **`README.md`** - Complete tutorial guide with learning path
- **`exercises/`** - Hands-on programming exercises with solutions
  - 01-basic-processes
  - 02-threads-intro
  - 03-synchronization
  - 04-ipc
  - 05-sockets
- **Example code by topic**:
  - `fork-examples/` - Process creation and management
  - `thread-examples/` - Threading basics
  - `synchronization-examples/` - Mutexes, condition variables, deadlocks
  - `ipc-examples/` - Inter-process communication
  - `commented-examples/` - Step-by-step annotated examples

See the README in this directory for build instructions and recommended learning path.

#### `assembly-examples/`
Organized collection of x86-64 AT&T syntax assembly examples. **Start here for assembly!**

Structure (progressive difficulty):
- `01-basics/` - Hello World and basic programs
- `02-data/` - Data sections, arrays, data types
- `03-control-flow/` - Loops, conditionals, branching
- `04-functions/` - Function calls and parameters
- `05-io/` - Input/output operations
- `06-advanced/` - AVX-512, floating-point, stack manipulation

See the README in this directory for the complete example index and assembly instructions.

### Specialized Topics

#### `file-intro/`
Introduction to file operations in C:
- Opening and reading files
- Writing to files
- Error handling
- Directory operations

#### `socket-intro/`
Network programming with sockets:
- TCP/IP socket programming
- Client-server architecture
- Basic network protocols

#### `rust-intro/`
Introduction to Rust programming language (8 .rs files)

#### `ipc-lecture/`
Inter-process communication concepts and examples

#### `deadlock-lecture/`
Understanding and preventing deadlocks in concurrent programs

### Reference Materials

#### `assemblyGuide.org`
Comprehensive guide to x86-64 assembly programming covering:
- Assembly basics and syntax
- Register usage and conventions
- Stack operations
- Function calling conventions
- Memory addressing modes
- I/O operations

#### `positTutorial.org`
Tutorial on posit number representation (alternative to floating-point)

#### Textbooks
- `Computer Systems GE.pdf` - Computer Systems textbook (5.6 MB)
- `diveintosystems.epub` - Dive Into Systems e-book (39.7 MB)

## Getting Started

### For Beginners
1. Start with `fall2024/lecture1` or `fall2025/lecture01` for basic C programming
2. Work through the semester materials in order
3. Reference `assemblyGuide.org` when starting assembly topics
4. Use the tutorial collections for deeper dives into specific topics

### For Concurrency Topics
1. Read `concurrency-tutorial/concurrencyGuide.org`
2. Work through the exercises in `concurrency-tutorial/exercises/`
3. Study the commented examples in `concurrency-tutorial/commented-examples/`
4. Reference the original examples for additional patterns

### For Assembly Programming
1. Read `assemblyGuide.org` for foundational concepts
2. Work through `assembly-examples/` starting with `01-basics/`
3. Study both the .c and .s versions to see C-to-assembly translation
4. Compile and run examples to understand execution

## Code Conventions

Throughout this repository you'll find:
- **Original files**: Core examples (e.g., `hello.c`, `fork1.c`)
- **Commented files**: Detailed explanations (e.g., `hellocommented.c`, `fork1commented.c`)
- **Source files**: `.c` for C, `.s` for assembly, `.rs` for Rust

## Building and Running Code

### C Programs
```bash
# Compile a C program
gcc filename.c -o filename

# With threading support
gcc filename.c -o filename -lpthread

# Run
./filename
```

### Assembly Programs
```bash
# Assemble and link
as filename.s -o filename.o
ld filename.o -o filename

# Or use gcc for easier linking
gcc filename.s -o filename

# Run
./filename
```

### Using Makefiles
Some directories include Makefiles for easier compilation:
```bash
make          # Build all examples
make clean    # Remove compiled files
```

## Additional Resources

- Online C compiler (for testing): https://www.onlinegdb.com/
- Course server: cslinux.pcc.edu (check with instructor for access)

## Questions or Issues?

If you find errors in the code or have questions about the materials, please reach out to your instructor or teaching assistant.

## License

These materials are provided for educational purposes as part of CS201.
