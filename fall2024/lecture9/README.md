# Lecture 9 - Processes, Threads, Signals, and Pipes

This directory contains C programs demonstrating fundamental concepts of concurrent programming including process creation, threading, inter-process communication, and signal handling.

## Contents

This lecture introduces parallel execution through processes and threads, along with mechanisms for communication and synchronization.

### Process Creation and Management
- **pid1.c** - Basic fork() demonstration
- **pid1commented.c** - Detailed explanation of process creation
- **pid2.c** - Process IDs and parent-child relationship
- **pid2commented.c** - Commented version explaining PIDs
- **pid3.c** - More complex fork example
- **pid3commented.c** - Advanced process patterns explained
- **pid4.c** - Multiple process creation
- **pid4commented.c** - Process tree explanation
- **forkCounter.c** - Counter with multiple processes
- **forkCountercommented.c** - Demonstrates process independence

### Thread Basics
- **thread1.c** - Basic pthread creation
- **thread1commented.c** - Introduction to threading
- **thread2.c** - Multiple threads
- **thread2commented.c** - Thread coordination explained
- **threadResult.c** - Getting return values from threads
- **threadResultcommented.c** - Thread join and results

### Thread Synchronization
- **badthreads.c** - Race condition demonstration
- **badthreadscommented.c** - Why synchronization is needed
- **threadmutex.c** - Mutex for thread safety
- **threadmutexcommented.c** - Mutex usage explained

### Signal Handling
- **signals1.c** - Basic signal handling
- **signals1commented.c** - Signal handler setup
- **signals2.c** - Custom signal handlers
- **signals2commented.c** - Handling different signals
- **signals3.c** - Signal masking and advanced handling
- **signals3commented.c** - Advanced signal concepts

### Inter-Process Communication
- **pipe1.c** - Basic pipe for IPC
- **pipe1commented.c** - Parent-child communication via pipes

### Additional Resources
- **in-class/** - Directory with in-class exercise examples

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
# Process examples
make pid1 pid2 pid3 pid4 forkCounter

# Thread examples
make thread1 thread2 threadResult badthreads threadmutex

# Signal examples
make signals1 signals2 signals3

# Pipe example
make pipe1
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Process Creation (pid1)
```bash
./pid1
```
**Expected behavior:**
- Creates one child process using fork()
- Both parent and child print messages with their PIDs
- Both processes execute the code after fork()
- Demonstrates that fork() returns different values to parent and child

### Process Counter (forkCounter)
```bash
./forkCounter
```
**Expected behavior:**
- Multiple processes increment a shared counter
- Demonstrates that processes have independent memory spaces
- Each process has its own copy of variables

### Basic Threading (thread1)
```bash
./thread1
```
**Expected behavior:**
- Creates threads that sleep for random durations (1-3 seconds)
- Each thread prints how long it slept
- Main thread waits for all threads to complete
- Demonstrates basic pthread_create() and pthread_join()

### Bad Threads (Race Condition)
```bash
./badthreads
```
**Expected behavior:**
- Demonstrates race condition with shared counter
- Multiple threads increment the same variable without synchronization
- Final count is often incorrect due to race conditions
- Shows why synchronization is necessary

### Thread Mutex
```bash
./threadmutex
```
**Expected behavior:**
- Multiple threads safely increment shared counter
- Uses mutex for synchronization
- Final count should be correct
- Demonstrates proper thread synchronization

### Signal Handling (signals1)
```bash
./signals1
```
**Expected behavior:**
- Installs custom signal handler
- Waits for signals (e.g., Ctrl+C sends SIGINT)
- Handler executes when signal received
- May exit gracefully or continue based on implementation

### Pipe Communication (pipe1)
```bash
./pipe1
```
**Expected behavior:**
- Parent process writes to pipe
- Child process reads from pipe
- Demonstrates unidirectional communication
- Shows file descriptor usage for IPC

## Key Concepts Demonstrated

### Process Management
- **fork()** - Creates child process by duplicating parent
  - Returns 0 to child, child PID to parent, -1 on error
- **getpid()** - Gets current process ID
- **getppid()** - Gets parent process ID
- **wait()** - Parent waits for child termination
- Process independence - separate memory spaces
- Process trees and hierarchies

### Threading Basics
- **pthread_create()** - Create new thread
  - Parameters: thread ID, attributes, function, arguments
- **pthread_join()** - Wait for thread completion
- Thread function signature: `void* function(void* arg)`
- Shared memory space between threads
- Thread IDs and thread management

### Thread Synchronization
- **Race conditions** - Concurrent access to shared data
- **Critical sections** - Code that accesses shared resources
- **Mutex (mutual exclusion)** - Lock mechanism
  - pthread_mutex_init() - Initialize mutex
  - pthread_mutex_lock() - Acquire lock
  - pthread_mutex_unlock() - Release lock
- Atomic operations importance

### Signal Handling
- **signal()** - Install signal handler
- **kill()** - Send signal to process
- Common signals:
  - SIGINT (Ctrl+C)
  - SIGUSR1, SIGUSR2 (user-defined)
  - SIGTERM (termination request)
- Signal handler function signature: `void handler(int sig)`
- Volatile variables in signal handlers

### Inter-Process Communication
- **pipe()** - Create unidirectional communication channel
- Returns two file descriptors: [0]=read end, [1]=write end
- Blocking I/O on pipes
- Closing unused pipe ends
- Parent-child communication patterns

## Troubleshooting

### Zombie Processes
If child processes become zombies:
```bash
ps aux | grep defunct
```
Always use `wait()` or `waitpid()` in parent to reap children.

### Thread Compilation Errors
Ensure you link pthread library:
```bash
gcc -pthread program.c -o program
```

### Race Conditions
- Results may vary between runs
- Use mutexes or other synchronization primitives
- Tools like `valgrind --tool=helgrind` can detect races

### Deadlocks
- Ensure locks are always released
- Acquire locks in consistent order
- Use timeouts when appropriate

## Notes

- **Processes vs Threads:**
  - Processes: separate memory, heavier weight, more isolation
  - Threads: shared memory, lighter weight, faster communication
- Fork creates complete copy of parent process
- Threads share all memory except their own stacks
- Race conditions are subtle and may not appear every time
- Signal handlers should be minimal and safe
- Pipes are buffered - writes may block if buffer full
- File descriptors 0, 1, 2 are stdin, stdout, stderr
- Always check return values for error handling
- The `-pthread` flag is required for threading programs
- Signal handlers can interrupt normal program flow
- Commented versions explain each system call and its purpose
- Understanding these concepts is crucial for systems programming
