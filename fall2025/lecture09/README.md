# Lecture 9 - Introduction to Processes and Threads

This directory contains examples demonstrating processes, threads, and concurrency for Fall 2025 Lecture 9.

## Contents

### Assembly Discussion
- **discussionDiscussion.c** - Assembly discussion examples in C
- **discussionDiscussioncommented.c** - Detailed explanation
- **discussionDiscussion.s** - Assembly version
- **discussionDiscussioncommented.s** - Detailed assembly explanation
- **dd2.s** - Additional discussion example
- **dd2commented.s** - Detailed explanation

### Process Examples (fork)
- **fork1.c** - Basic fork demonstration
- **fork1commented.c** - Detailed explanation of process creation
- **fork2.c** - Parent/child process distinction
- **fork2commented.c** - Detailed explanation of PID handling
- **fork3.c** - Process waiting and synchronization
- **fork3commented.c** - Detailed explanation of wait()

### Thread Examples (pthreads)
- **thread1.c** - Basic thread creation and joining
- **thread1commented.c** - Detailed explanation of pthread basics
- **thread2.c** - Thread with arguments
- **thread2commented.c** - Detailed explanation of passing data to threads

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make fork1
make fork2
make thread1
make thread2
```

**Note:** Thread programs require linking with pthread library:
```bash
gcc thread1.c -o thread1 -pthread
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic Fork
```bash
./fork1
```

**Expected behavior:**
- Prints "How's it going?" once
- Prints "Hi there" twice (once from parent, once from child)
- Demonstrates that fork() creates a complete copy of the process

### Parent/Child Distinction
```bash
./fork2
```

**Expected behavior:**
- Parent and child execute different code paths
- Uses return value of fork() to distinguish:
  - Parent receives child's PID (positive number)
  - Child receives 0
- Shows how to coordinate parent and child behavior

### Process Waiting
```bash
./fork3
```

**Expected behavior:**
- Parent waits for child to complete
- Demonstrates wait() or waitpid() system calls
- Shows proper process synchronization
- Parent doesn't exit until child finishes

### Basic Thread
```bash
./thread1
```

**Expected behavior:**
- Creates new thread that runs thread_function
- Main thread waits for worker thread to complete
- Prints "Thread executing" from worker thread
- Prints "Thread joined" after thread completes
- Shows basic pthread_create and pthread_join

### Thread with Arguments
```bash
./thread2
```

**Expected behavior:**
- Passes data to thread via argument
- Demonstrates void* casting for thread arguments
- Shows how to share data between threads
- Worker thread can access data from main thread

## Key Concepts Demonstrated

### Processes
- **Process** - Operating system's unit of program execution
- **fork()** - Creates exact copy of calling process
  - Returns twice: once in parent, once in child
  - Parent receives child's PID
  - Child receives 0
- **Complete copy** - Child gets copy of all memory, file descriptors, etc.
- **Separate address spaces** - Changes in child don't affect parent

### Process IDs
- **PID** - Process ID (unique identifier for each process)
- **PPID** - Parent Process ID
- **getpid()** - Returns current process's PID
- **getppid()** - Returns parent's PID

### Process Synchronization
- **wait()** - Parent waits for any child to terminate
- **waitpid()** - Parent waits for specific child
- **Exit status** - Child's return value captured by parent
- **Zombie processes** - Children whose parent hasn't called wait()

### Threads
- **Thread** - Lightweight unit of execution within a process
- **Shared address space** - All threads share same memory
- **Separate execution** - Each thread has own registers and stack
- **pthread_t** - Thread identifier type

### Thread Functions
- **pthread_create()** - Creates new thread
  - Args: thread ID, attributes, function, argument
  - Function signature: `void* func(void* arg)`
- **pthread_join()** - Wait for thread to complete
  - Args: thread ID, return value pointer
  - Blocks until specified thread exits

### Threads vs Processes
**Processes:**
- Separate address spaces
- Heavy creation overhead (full copy)
- IPC needed for communication
- More isolation (safer)
- More resource usage

**Threads:**
- Shared address space
- Light creation overhead
- Direct memory sharing
- Less isolation (race conditions possible)
- More efficient for parallel tasks

### Shared vs Separate
**In Processes (Separate):**
- Memory (heap, stack, globals)
- File descriptor table (copied at fork)
- Environment variables
- Working directory

**In Threads (Shared):**
- Heap memory
- Global variables
- File descriptors
- Code section

**In Threads (Separate):**
- Registers (including %rip)
- Stack
- Thread-local storage

### Function Pointers
- Thread functions use function pointers
- Signature: `void* (*func)(void* arg)`
- Allows generic interface for any thread function
- Requires casting arguments to/from `void*`

### Race Conditions
- Occur when multiple threads access shared data
- Order of execution matters
- Results can be non-deterministic
- Need synchronization (mutexes, semaphores, etc.)

## Notes

- fork() creates processes, pthread_create() creates threads
- fork() is expensive (full copy), threads are lightweight
- Threads share memory, making communication easy but dangerous
- Always join threads or detach them (prevent resource leaks)
- Always wait for child processes (prevent zombie processes)
- Thread programs must link with -pthread flag
- Race conditions are a major concern with threads
- Debugging concurrent programs is challenging
- Output order may vary between runs due to scheduling

## Troubleshooting

### Zombie processes appearing:
- Parent must call wait() or waitpid() for each child
- Use `ps aux | grep Z` to see zombie processes
- Zombies disappear when parent exits or calls wait()

### Thread program won't compile:
- Add `-pthread` flag: `gcc prog.c -o prog -pthread`
- Include `<pthread.h>` header
- Check function signature matches `void* func(void* arg)`

### Unexpected output from fork():
- Remember both processes execute code after fork()
- Check return value to distinguish parent (>0) from child (0)
- Use fflush(stdout) before fork() to avoid duplicate buffered output

### Thread terminates before executing:
- Main thread must call pthread_join() to wait
- If main exits, all threads are killed
- Use pthread_join() for each created thread

### Segmentation fault in thread:
- Check argument passing and casting
- Verify shared data is properly allocated
- Don't pass pointers to stack variables that go out of scope

## Resources

- Chapter 12 of "Dive into Systems" - Processes
- Chapter 13 of "Dive into Systems" - Threads
- POSIX Threads Programming guide
- `man fork`, `man pthread_create`, `man pthread_join`
