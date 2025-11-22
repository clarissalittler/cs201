# Lecture 9 - Introduction to POSIX Threads

This directory contains examples for Winter 2025 Lecture 9, introducing multithreading with POSIX threads (pthreads), demonstrating thread creation, synchronization, and race conditions.

## Contents

### Basic Threading
- **threads1.c** - Creating and joining basic threads
- **threads1commented.c** - Detailed explanation of pthread basics
- **threads2.c** - Passing arguments to threads
- **threads2commented.c** - Complete guide to thread arguments
- **threads3.c** - Multiple threads with shared data
- **threads3commented.c** - Demonstrates race conditions
- **threads4.c** - Thread-local variables and function pointers
- **threads4commented.c** - Advanced thread concepts

### Thread Synchronization
- **threadmutex.c** - Mutex for synchronization
- **threadmutexcommented.c** - Comprehensive mutex tutorial

## Compilation

### Compile all examples:
```bash
make
```

**Note:** Must link with pthread library using `-pthread` flag.

### Compile specific programs:
```bash
make threads1
make threadmutex
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic Thread Creation
```bash
./threads1
```
**Expected output:**
```
Hi I'm a thread!
Hi I'm a thread!
```
Two threads print their message.

### Threads with Arguments
```bash
./threads2
```
**Expected output:**
Shows threads receiving and processing arguments.

### Shared Data (Race Condition Demo)
```bash
./threads3
```
**Expected behavior:**
- Multiple threads increment shared counter
- Without synchronization, final count is unpredictable
- Demonstrates race condition problem
- Run multiple times to see different results

### Thread Synchronization (Mutex)
```bash
./threadmutex
```
**Expected behavior:**
- Threads use mutex to safely access shared data
- Final count is always correct
- Demonstrates proper synchronization

## Key Concepts Demonstrated

### What is a Thread?

**Thread vs Process:**
- **Process**: Independent program with own memory space
- **Thread**: Lightweight execution unit sharing process memory

**Advantages of threads over processes:**
- Much faster to create
- Much less memory overhead
- Easy data sharing (shared memory space)
- Better for concurrent tasks

**Challenges with threads:**
- Must synchronize access to shared data
- Race conditions can occur
- Debugging is more complex

### Creating Threads

```c
#include <pthread.h>

void* thread_function(void* arg) {
    // Thread code here
    printf("Hello from thread!\n");
    return NULL;
}

int main() {
    pthread_t thread_id;

    // Create thread
    pthread_create(&thread_id, NULL, thread_function, NULL);

    // Wait for thread to finish
    pthread_join(thread_id, NULL);

    return 0;
}
```

### pthread_create() Parameters
1. **pthread_t* thread**: Pointer to thread identifier
2. **pthread_attr_t* attr**: Thread attributes (NULL for defaults)
3. **void* (*start_routine)(void*)**: Function to execute
4. **void* arg**: Argument passed to function

### Thread Function Signature

```c
void* function_name(void* arg)
```

- **void* arg**: Polymorphic argument (can be any pointer type)
- **return value**: void* (can return any pointer)

### Passing Arguments to Threads

**Single argument:**
```c
int value = 42;
pthread_create(&thread, NULL, func, &value);
```

**Multiple arguments (using struct):**
```c
struct thread_args {
    int id;
    char* message;
};

struct thread_args args = {1, "Hello"};
pthread_create(&thread, NULL, func, &args);
```

**Important:** Arguments must remain valid for thread's lifetime!

### Joining Threads

```c
void* return_value;
pthread_join(thread_id, &return_value);
```

- Waits for thread to complete
- Retrieves return value (if needed)
- Similar to wait() for processes
- Must join or detach every thread

### Detached Threads

```c
pthread_detach(thread_id);
```

- Thread cleans up automatically when done
- Cannot join detached thread
- Useful for fire-and-forget tasks

### Race Conditions

**Problem: Multiple threads accessing shared data**

```c
int counter = 0;  // Shared variable

void* increment(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;  // NOT ATOMIC!
    }
    return NULL;
}
```

**Why it fails:**
```
Thread 1: Read counter (0)
Thread 2: Read counter (0)
Thread 1: Increment (1)
Thread 2: Increment (1)
Thread 1: Write counter (1)
Thread 2: Write counter (1)
Result: counter = 1 (should be 2!)
```

### Mutex (Mutual Exclusion)

**Solution to race conditions:**

```c
#include <pthread.h>

pthread_mutex_t lock;

// Initialize mutex
pthread_mutex_init(&lock, NULL);

void* safe_increment(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

// Cleanup
pthread_mutex_destroy(&lock);
```

### Mutex Operations

- **pthread_mutex_init(&mutex, attr)**: Initialize mutex
- **pthread_mutex_lock(&mutex)**: Acquire lock (blocks if locked)
- **pthread_mutex_trylock(&mutex)**: Try to lock (returns immediately)
- **pthread_mutex_unlock(&mutex)**: Release lock
- **pthread_mutex_destroy(&mutex)**: Clean up mutex

### Critical Section

Code protected by mutex:

```c
pthread_mutex_lock(&lock);
// Critical section - only one thread at a time
shared_data++;
pthread_mutex_unlock(&lock);
```

**Rules:**
- Keep critical sections short
- Never return or exit without unlocking
- Avoid calling other functions that might lock

### Thread-Safe Programming

**Guidelines:**
1. Identify shared data
2. Protect with mutexes
3. Minimize critical section size
4. Avoid nested locks (deadlock risk)
5. Use const for read-only shared data

### Compilation Note

Always compile with `-pthread`:

```bash
gcc -pthread threads1.c -o threads1
```

or in Makefile:
```makefile
CFLAGS = -pthread
```

## Common Problems and Solutions

### Segmentation Fault?
- Check argument lifetime (don't pass address of local variable)
- Ensure proper synchronization
- Verify no null pointer dereferences

### Inconsistent Results?
- Likely race condition
- Add mutex protection
- Check all shared data access points

### Deadlock (program hangs)?
- Thread waiting for mutex held by another thread
- Check for circular lock dependencies
- Always lock in same order
- Use pthread_mutex_trylock() to avoid blocking

### Memory Leaks?
- Always join or detach threads
- Clean up mutexes with pthread_mutex_destroy()
- Free any allocated memory in threads

## Performance Considerations

**When to use threads:**
- I/O-bound operations (waiting for network, disk)
- CPU-bound with multiple cores
- Concurrent independent tasks

**Overhead:**
- Thread creation: ~microseconds
- Context switching: overhead
- Mutex locking: some overhead

**Best practices:**
- Use thread pools for many short tasks
- Minimize lock contention
- Consider lock-free data structures for high performance

## Troubleshooting

### Program terminates before threads finish?
Add pthread_join() to wait for threads.

### Wrong values in shared variables?
Add mutex protection around shared data access.

### Compile error "undefined reference to pthread_create"?
Add `-pthread` flag to compilation command.

## Notes

- Threads share: code, heap, global variables, file descriptors
- Threads have separate: stack, registers, thread ID
- void* enables polymorphism in C (type safety is your responsibility)
- Always initialize mutexes before use
- Modern CPUs execute threads truly in parallel on multiple cores
- Understanding threads is essential for modern systems programming
- Next lecture covers condition variables and advanced synchronization
