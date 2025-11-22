# Forking - Process Management and IPC

This directory serves as a guide to fork-based process management and inter-process communication examples located throughout the repository.

## Overview

This directory contains an agenda/reference guide that points to various fork examples and IPC mechanisms across the codebase. The topics covered include basic process forking, process synchronization, exit status handling, and inter-process communication.

## Contents

- **Agenda.org** - Comprehensive guide linking to fork and IPC examples throughout the repository

## Topics Covered

### 1. Basic Fork Concepts
Understanding how `fork()` creates new processes:
- Process duplication in the operating system
- How parent and child processes relate
- Memory copying and process independence

**Key examples referenced:**
- Basic fork demonstrations
- Parent/child process identification
- Process ID (PID) access

### 2. Process Creation Patterns
Multiple approaches to creating and managing processes:
- Simple fork patterns
- Creating process trees with multiple forks
- Variable behavior after fork (copy-on-write)

### 3. Process Synchronization
Preventing zombie processes and race conditions:
- Using `wait()` and `waitpid()`
- Ensuring parent waits for child completion
- Proper error handling in fork operations
- Avoiding orphaned processes

### 4. Exit Status Handling
Managing process termination:
- Retrieving exit status from child processes
- Using `WEXITSTATUS()` macro correctly
- Understanding raw vs processed exit codes
- Common pitfalls in exit status checking

### 5. Inter-Process Communication (IPC)
Synchronizing data between forked processes:
- **Pipes** - Unidirectional communication channels
- **Named Pipes (FIFOs)** - Bidirectional communication
- **Shared Memory** - High-performance data sharing
- Producer-consumer patterns

## Finding the Examples

The Agenda.org file contains links to examples in these locations:

### Fork Examples
```
/home/user/cs201/concurrency-tutorial/fork-examples/
├── fork1.c - Basic fork
├── fork2.c - Parent/child identification
├── fork3.c - Fork with wait
├── fork4.c - Exit status
├── fork5.c - User input exit
└── pid*.c - Process ID examples
```

### IPC Examples
```
/home/user/cs201/concurrency-tutorial/ipc-examples/
├── pipe1.c - Simple pipe
├── namedPipesServer1.c - FIFO server
├── namedPipesClient1.c - FIFO client
├── sharedMemprod.c - Shared memory producer
└── sharedMemcons.c - Shared memory consumer
```

### Lecture Examples
```
/home/user/cs201/winter2025/lecture9/in-class/
├── fork1.c - Classroom fork example
├── fork2.c - Multiple forks
├── fork3.c - Fork with error handling
├── fork5.c - Fork with variables
└── pipe1.c - Simple pipe demonstration
```

## Compilation

For the referenced C programs, use:

```bash
# Navigate to the example directory
cd /home/user/cs201/concurrency-tutorial/fork-examples/

# Compile a fork example
gcc fork1.c -o fork1

# Or compile with warnings and debugging
gcc -Wall -g fork1.c -o fork1
```

## Running Fork Examples

### Basic Fork (fork1.c)
```bash
./fork1
```
**Expected behavior:**
- Creates one child process
- Both parent and child print the same message
- You'll see the message printed twice

### Fork with Wait (fork3.c)
```bash
./fork3
```
**Expected behavior:**
- Parent creates child process
- Parent waits for child to complete
- Orderly output showing synchronization

### Exit Status (fork4.c)
```bash
./fork4
```
**Expected behavior:**
- Child exits with a specific status code
- Parent retrieves and displays the exit status

## Running IPC Examples

### Pipe Example
```bash
./pipe1
```
**Expected behavior:**
- Parent writes to pipe
- Child reads from pipe
- Demonstrates unidirectional data flow

### Named Pipes (FIFOs)

**Terminal 1 (Server):**
```bash
./namedPipesServer1
```

**Terminal 2 (Client):**
```bash
./namedPipesClient1
```

**Expected behavior:**
- Bidirectional communication between processes
- Server and client can both send and receive messages

### Shared Memory

**Terminal 1 (Producer):**
```bash
./sharedMemprod
```

**Terminal 2 (Consumer):**
```bash
./sharedMemcons
```

**Expected behavior:**
- Producer writes data to shared memory
- Consumer reads data from shared memory
- High-performance data sharing

## Key Concepts

### Fork Mechanics
- `fork()` returns twice: once in parent (child PID), once in child (0)
- Child is a duplicate of parent at time of fork
- Each process has its own memory space (copy-on-write)
- Both processes continue from the same point

### Process States
- **Running** - Actively executing
- **Waiting** - Blocked on I/O or synchronization
- **Zombie** - Terminated but exit status not yet collected
- **Orphan** - Parent terminated before child

### IPC Mechanisms

**Pipes:**
- Unidirectional byte streams
- Used between related processes
- Automatic cleanup when processes exit

**Named Pipes (FIFOs):**
- Persistent in filesystem
- Can connect unrelated processes
- Support bidirectional communication with two FIFOs

**Shared Memory:**
- Fastest IPC mechanism
- Requires synchronization (semaphores/mutexes)
- Manual cleanup required

## Troubleshooting

### Zombie Processes
```bash
# If you see defunct/zombie processes:
ps aux | grep defunct

# Fix: Ensure parent calls wait() or waitpid()
# Or handle SIGCHLD signal properly
```

### Orphaned Processes
```bash
# If parent exits before child:
# Child is adopted by init (PID 1)
# Usually harmless but can be prevented with proper wait()
```

### Pipe Deadlocks
```bash
# If program hangs on pipe operations:
# 1. Ensure writer closes write end after writing
# 2. Ensure reader closes read end after reading
# 3. Don't have both processes waiting for each other
```

### Shared Memory Cleanup
```bash
# List shared memory segments
ipcs -m

# Remove orphaned segment
ipcrm -m <shmid>
```

## Notes

- This directory serves as a navigation/reference guide rather than containing executable code
- All code examples are located in other directories as referenced in Agenda.org
- Fork-based concurrency is the oldest Unix concurrency model
- Modern programs often prefer threads for shared-memory concurrency
- IPC mechanisms each have trade-offs in speed, complexity, and use cases
- Always clean up IPC resources to avoid system resource leaks

## Related Topics

See also:
- `/home/user/cs201/spring2025/threading/` - Thread-based concurrency (alternative to forks)
- `/home/user/cs201/winter2025/lecture13/` - Advanced IPC with message queues and sockets
- `/home/user/cs201/concurrency-tutorial/` - Comprehensive concurrency examples
