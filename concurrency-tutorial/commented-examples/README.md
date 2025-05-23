# Concurrency Programming in C - Commented Examples

This directory contains thoroughly commented example programs that demonstrate key concepts in concurrency programming with C. Each example is designed to be educational, with extensive comments explaining the concepts, implementation details, and best practices.

## Directory Structure

The examples are organized in a logical progression from basic to advanced concepts:

1. **Basic Processes**
   - Introduction to fork() and process creation
   - Process identification and relationships
   - Process termination and waiting for children

2. **Advanced Processes**
   - The exec() family of functions
   - Process trees and exponential process creation

3. **Basic Threads**
   - Thread creation and joining
   - Passing arguments to threads
   - Returning values from threads

4. **Thread Synchronization**
   - Mutex basics and race conditions
   - Condition variables for signaling
   - Deadlocks and prevention techniques

5. **Inter-Process Communication (IPC)**
   - Pipes for related processes
   - Named pipes (FIFOs) for unrelated processes
   - Shared memory for high-performance data sharing
   - Unix domain sockets for flexible communication

6. **Socket Programming** (see main examples and exercises)
   - TCP/IP socket basics
   - Client-server architecture
   - Multi-client servers
   - Socket I/O multiplexing with select()

## How to Use These Examples

Each example is self-contained and includes:

1. A description of the concept being demonstrated
2. Detailed comments explaining each part of the code
3. A comprehensive explanation section covering the theoretical aspects
4. Information about compilation and execution

To compile and run an example:

```bash
# Compile
gcc -o example_name example_name.c -pthread

# Run
./example_name
```

For client-server examples (like named pipes, shared memory, and sockets), you'll need to run two instances in separate terminals as described in each example's usage instructions.

## Learning Approach

For the best learning experience:

1. Start with the basic process examples and work your way through in order
2. Read all comments before running each program
3. Experiment by modifying the examples to deepen your understanding
4. Refer to the comprehensive explanation section in each file for theoretical background

## Key Concepts Covered

- Process vs. thread models
- Creating, managing, and synchronizing concurrent units of execution
- Race conditions and critical sections
- Synchronization primitives (mutexes, condition variables, semaphores)
- Various IPC mechanisms and their trade-offs
- Common concurrency patterns (producer-consumer, readers-writers)
- Deadlock causes and prevention
- Safe and efficient concurrent programming techniques
- Network programming with TCP/IP sockets
- Building scalable client-server applications

Each example aims to provide a clear, practical demonstration of these concepts with thorough explanations that bridge theory and implementation.