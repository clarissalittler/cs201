# Lecture 7 - Processes, Forking, and Signals

This directory contains examples for Winter 2025 Lecture 7, demonstrating process creation, inter-process communication, function pointers, and signal handling.

## Contents

### Process Creation and Management
- **pid1.c** - Basic fork() demonstration
- **pid1commented.c** - Detailed explanation of process creation
- **pid2.c** - Fork with wait() for child processes
- **pid2commented.c** - Complete explanation of parent-child synchronization
- **pid3.c** - Multiple child processes
- **pid3commented.c** - Managing multiple children with wait()
- **pid4.c** - Advanced process control with exit status
- **pid4commented.c** - Extracting and using child exit codes
- **noparent.c** - Demonstrates orphan process behavior
- **noparentcommented.c** - Explains process lifecycle

### Signal Handling
- **sig1.c** - Basic signal handling (SIGINT)
- **sig1commented.c** - Complete explanation of signal handlers
- **sig2.c** - Multiple signal types
- **sig2commented.c** - Handling different signals
- **tag.c** - Parent-child signal communication
- **tagcommented.c** - Bidirectional signaling between processes

### Function Pointers and Advanced C
- **pic3.c** - Function pointer demonstration
- **pic3commented.c** - Detailed explanation of function pointers
- **leaQuestion.s** - LEA (Load Effective Address) instruction example
- **leaQuestioncommented.s** - Assembly explanation of address calculation

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make pid1
make sig1
make tag
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic Fork
```bash
./pid1
```
**Expected output:**
- Shows both parent and child process output
- Demonstrates process ID (PID) differences
- Parent PID is non-zero, child sees 0 from fork()

### Fork with Wait
```bash
./pid2
```
**Expected output:**
- Parent waits for child to complete
- Ordered output showing parent and child coordination

### Multiple Children
```bash
./pid3
```
**Expected output:**
- Parent creates multiple children
- Each child has unique PID
- Parent waits for all children

### Exit Status Handling
```bash
./pid4
```
**Expected output:**
- Child exits with specific code
- Parent retrieves and displays exit status using WEXITSTATUS

### Signal Handling
```bash
./sig1
```
**Expected behavior:**
- Press Ctrl+C (SIGINT)
- Custom signal handler executes instead of terminating
- Program continues running

```bash
./sig2
```
**Expected behavior:**
- Handles multiple signal types
- Demonstrates signal masking and delivery

### Process Communication (Tag)
```bash
./tag
```
**Expected behavior:**
- Parent and child exchange signals
- Demonstrates SIGUSR1 and SIGUSR2
- Bidirectional communication pattern

## Key Concepts Demonstrated

### Process Creation with fork()

```c
pid_t pid = fork();

if (pid == -1) {
    // Error occurred
    perror("fork");
} else if (pid == 0) {
    // Child process
    printf("I'm the child!\n");
} else {
    // Parent process (pid contains child's PID)
    printf("I'm the parent, child PID is %d\n", pid);
}
```

### Key fork() Behaviors
- Creates exact copy of parent process
- Both processes continue from same point (after fork())
- Child receives copy of parent's memory, file descriptors, etc.
- Return value differs: 0 in child, child's PID in parent, -1 on error
- Both processes run concurrently

### Waiting for Children

```c
int status;
pid_t child_pid = wait(&status);

if (WIFEXITED(status)) {
    int exit_code = WEXITSTATUS(status);
    printf("Child exited with code: %d\n", exit_code);
}
```

### wait() Family Functions
- **wait(&status)**: Wait for any child
- **waitpid(pid, &status, options)**: Wait for specific child
- **WEXITSTATUS(status)**: Extract exit code (bits 8-15)
- **WIFEXITED(status)**: Check if exited normally

### Signal Handling

```c
#include <signal.h>

void handler(int signum) {
    printf("Caught signal %d\n", signum);
}

signal(SIGINT, handler);  // Register handler
// or
struct sigaction sa;
sa.sa_handler = handler;
sigaction(SIGINT, &sa, NULL);  // More robust
```

### Common Signals
- **SIGINT (2)**: Interrupt (Ctrl+C)
- **SIGTERM (15)**: Termination request
- **SIGUSR1 (10)**: User-defined signal 1
- **SIGUSR2 (12)**: User-defined signal 2
- **SIGKILL (9)**: Kill (cannot be caught)
- **SIGSTOP (19)**: Stop (cannot be caught)

### Sending Signals
```c
kill(pid, SIGTERM);    // Send signal to process
raise(SIGINT);         // Send signal to self
```

### Function Pointers

```c
// Define function pointer type
int (*func_ptr)(int, int);

// Assign function to pointer
func_ptr = &my_function;  // or just: func_ptr = my_function;

// Call via pointer
int result = (*func_ptr)(5, 10);  // or: func_ptr(5, 10);
```

**Use cases:**
- Callbacks
- Signal handlers
- Polymorphism in C
- Jump tables
- Plugin systems

### Process Memory Layout
When a process runs on Linux:
- **Text segment**: Executable code (read-only)
- **Data segment**: Initialized global variables
- **BSS segment**: Uninitialized global variables
- **Heap**: Dynamic memory (malloc/free)
- **Stack**: Local variables, function calls
- **Memory-mapped regions**: Shared libraries, etc.

## Process Lifecycle

1. **Running**: Process is executing
2. **Waiting**: Blocked on I/O or signal
3. **Stopped**: Suspended (SIGSTOP)
4. **Zombie**: Exited but parent hasn't wait()ed
5. **Terminated**: Fully cleaned up

### Orphan Processes
- Parent dies before child
- Child adopted by init (PID 1)
- See noparent.c for demonstration

### Zombie Processes
- Child exits but parent doesn't wait()
- Process entry remains in process table
- Shows as `<defunct>` in ps
- Always wait() for your children!

## Troubleshooting

### Zombie processes accumulating?
```bash
ps aux | grep defunct
```
Fix: Ensure parent calls wait() or waitpid()

### Signal handler not working?
- Check signal number is correct
- Verify handler is registered before signal arrives
- Some signals (SIGKILL, SIGSTOP) cannot be caught

### Fork fails?
- Check system resource limits (ulimit -a)
- May have too many processes running
- Insufficient memory

## Notes

- fork() is expensive - creates full process copy
- Use vfork() or clone() for special cases (advanced)
- Always check return values from fork(), wait(), etc.
- Signal handlers should be short and simple
- Avoid calling non-reentrant functions in signal handlers
- Function pointers enable powerful abstraction patterns
- Understanding processes is fundamental to Unix/Linux programming
- Modern alternatives: threads (lighter weight than processes)
