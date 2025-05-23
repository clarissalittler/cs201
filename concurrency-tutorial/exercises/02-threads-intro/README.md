# Thread Introduction Exercises

This section focuses on basic thread concepts: creation, management, and fundamental coordination.

## Learning Objectives

After completing these exercises, you will be able to:
- Create and manage threads using pthread library
- Pass arguments to threads and retrieve return values
- Understand thread vs process differences
- Handle basic thread coordination and joining

## Prerequisites

- Completion of basic process exercises
- Understanding of pointers and structures in C
- Familiarity with pthread library concepts

## Exercises

### Exercise 1: First Thread 🟢
**File**: `exercise1.c`
**Concepts**: Basic thread creation, pthread_join

Create a program that spawns a single thread. Both main thread and child thread should print their thread IDs and a message.

### Exercise 2: Thread Arguments 🟢
**File**: `exercise2.c`
**Concepts**: Passing data to threads, thread functions

Write a program that creates 3 threads, each receiving a different integer argument. Each thread should print its argument and thread ID.

### Exercise 3: Thread Return Values 🟡
**File**: `exercise3.c`
**Concepts**: Returning data from threads, pointer handling

Create threads that perform simple calculations and return results to the main thread. The main thread should collect and print all results.

### Exercise 4: Shared Counter Problem 🟡
**File**: `exercise4.c`
**Concepts**: Race conditions, shared memory issues

Create multiple threads that increment a shared counter. Observe the race condition and understand why the final result is unpredictable.

### Exercise 5: Thread Pool Pattern 🔴
**File**: `exercise5.c`
**Concepts**: Thread pools, work distribution, coordination

Implement a simple thread pool where multiple worker threads process tasks from a shared work queue (without synchronization - to observe problems).

## Key Differences: Threads vs Processes

| Aspect | Processes | Threads |
|--------|-----------|---------|
| Memory | Separate address spaces | Shared address space |
| Creation | fork() system call | pthread_create() |
| Communication | IPC mechanisms | Shared variables |
| Isolation | High (protected) | Low (shared memory) |
| Overhead | Higher | Lower |
| Debugging | Easier | More complex |

## Compilation

```bash
gcc -o exercise1 exercise1.c -pthread
gcc -o exercise2 exercise2.c -pthread
# ... etc
```

**Note**: The `-pthread` flag is essential for linking the pthread library.

## Tips

- Always join threads with `pthread_join()` before main() exits
- Pass data to threads through void* parameters
- Use structures to pass multiple values to threads
- Remember that threads share memory - changes are visible to all threads
- Thread IDs are of type `pthread_t`, not integers like PIDs