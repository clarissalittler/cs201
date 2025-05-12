# Concurrency Programming Tutorial

A comprehensive guide to concurrency programming in C, focusing on processes, threads, synchronization, and inter-process communication (IPC).

## Contents

This tutorial collection contains:

1. **concurrencyGuide.org** - A detailed theoretical guide to concurrency concepts
2. **Example code** organized by topic:
   - `fork-examples/` - Process creation and management
   - `thread-examples/` - Thread creation and management
   - `synchronization-examples/` - Mutexes, condition variables, and deadlock prevention
   - `ipc-examples/` - Inter-process communication mechanisms
3. **EXAMPLE_INDEX.md** - A catalog of all original examples with descriptions of concepts demonstrated
4. **Thoroughly commented examples** with step-by-step explanations:
   - `commented-examples/` - Progressive learning path with detailed annotations

## Getting Started

### Prerequisites

- A Linux or Unix-like system (most examples use POSIX APIs)
- GCC compiler
- Basic knowledge of C programming

### Building Examples

To build all examples:

```bash
# Build everything (both original and commented examples)
make

# Build only the original examples
make original

# Build only the commented examples
make commented
```

All compiled executables will be placed in the `bin/` directory, organized by category.

### Running Examples

After building, you can run any example by executing the corresponding binary:

```bash
# For original examples
./bin/fork-examples/fork1
./bin/thread-examples/thread1
# etc.

# For commented examples
./bin/commented-examples/01-basic-processes/01-fork-basics
./bin/commented-examples/03-basic-threads/01-thread-basics
# etc.
```

### Client-Server Examples

Some examples demonstrate client-server patterns (like named pipes, shared memory, and sockets). For these, you'll need to run two separate instances in different terminals:

```bash
# Terminal 1 (server)
./bin/commented-examples/05-ipc/02-named-pipes server

# Terminal 2 (client)
./bin/commented-examples/05-ipc/02-named-pipes client
```

## Learning Path

For beginners, we recommend following this learning sequence:

1. Start with the `concurrencyGuide.org` document for theoretical background
2. Browse through the `EXAMPLE_INDEX.md` to understand what concepts each example demonstrates
3. Explore the commented examples in this order:
   - Basic processes (`01-basic-processes/`)
   - Advanced processes (`02-advanced-processes/`)
   - Basic threads (`03-basic-threads/`)
   - Thread synchronization (`04-thread-synchronization/`)
   - Inter-process communication (`05-ipc/`)
4. Refer to the original examples for additional variants and techniques

## Troubleshooting

If you encounter build issues:

```bash
# Run the build test script to check for compilation errors
./build_test.sh
```

## Key Topics Covered

- Process creation with `fork()`
- Process execution with `exec()` family
- Thread creation and management with pthreads
- Synchronization mechanisms:
  - Mutexes
  - Condition variables
  - Semaphores
- Race conditions and deadlocks
- Inter-process communication:
  - Pipes
  - Named pipes (FIFOs)
  - Shared memory
  - Unix domain sockets

## License

This tutorial and code examples are provided for educational purposes.

## Acknowledgments

This tutorial is designed for CS201 to provide practical examples of concurrency programming concepts.