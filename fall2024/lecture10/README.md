# Lecture 10 - Advanced Threading Concepts

This directory contains C programs demonstrating various threading patterns, thread management, and advanced concurrent programming techniques.

## Contents

This lecture deepens understanding of POSIX threads (pthreads) with more complex examples and patterns.

### Threading Examples

- **threads1.c** - Simple thread creation with random sleep
- **threads1commented.c** - Basic threading concepts explained
- **threads2.c** - Multiple threads with shared behavior
- **threads2commented.c** - Thread coordination patterns
- **threads3.c** - Threads with arguments and data passing
- **threads3commented.c** - Passing data to thread functions explained
- **threads4.c** - Advanced thread patterns
- **threads4commented.c** - Complex thread interaction explained
- **goofy.c** - Creative/experimental threading example
- **goofycommented.c** - Unconventional thread usage explained

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make threads1
make threads2
make threads3
make threads4
make goofy
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Threads1 - Basic Thread Creation
```bash
./threads1
```

**Expected behavior:**
- Creates two threads
- Each thread sleeps for a random duration (1-3 seconds)
- Threads print how long they slept
- Main thread waits for both threads to complete
- Output order may vary between runs (demonstrates concurrency)

**Sample output:**
```
I slept for 2 seconds
I slept for 1 seconds
```
(Order depends on which thread finishes first)

### Threads2 - Multiple Threads
```bash
./threads2
```

**Expected behavior:**
- Creates multiple threads with similar tasks
- Demonstrates concurrent execution
- Shows thread interleaving
- All threads share the same thread function

### Threads3 - Threads with Arguments
```bash
./threads3
```

**Expected behavior:**
- Passes arguments to thread functions
- Each thread may have different data
- Demonstrates parameter passing mechanism
- Shows how threads can work on different data

### Threads4 - Advanced Patterns
```bash
./threads4
```

**Expected behavior:**
- Demonstrates more complex threading scenarios
- May include synchronization or coordination
- Shows practical threading patterns

### Goofy - Experimental Threading
```bash
./goofy
```

**Expected behavior:**
- Demonstrates creative or unusual thread usage
- May show edge cases or interesting behaviors
- Educational example of what's possible with threads

## Key Concepts Demonstrated

### Thread Creation and Management
- **pthread_create()** - Create new thread
  ```c
  pthread_create(&thread_id, NULL, thread_function, argument);
  ```
- **pthread_join()** - Wait for thread completion
  ```c
  pthread_join(thread_id, &return_value);
  ```
- Thread lifecycle: creation, execution, termination
- Main thread vs worker threads

### Thread Functions
- Function signature: `void* function_name(void* arg)`
- Return type is `void*` for flexibility
- Arguments passed as `void*` (must be cast appropriately)
- Returning values through `return` or pthread_exit()

### Passing Data to Threads
- Single argument via `void*` pointer
- Passing integers by casting
- Passing structures for multiple values
- Avoiding stack-allocated data (use heap or static)

### Thread Synchronization
- Implicit synchronization with pthread_join()
- Understanding when synchronization is needed
- Shared memory access patterns
- Thread-safe programming practices

### Concurrency Patterns
- Independent parallel tasks
- Thread pools (conceptually)
- Producer-consumer patterns
- Work distribution across threads

### Random Number Generation in Threads
- Seeding random number generator
- `srand(time(0))` for initialization
- Different seeds for different threads
- Thread-safety considerations

## Troubleshooting

### Compilation Issues
Always use the `-pthread` flag:
```bash
gcc -pthread threads1.c -o threads1
```

### Segmentation Faults
Common causes:
- Passing stack-allocated data to threads
- Accessing freed memory
- Race conditions on shared data

**Fix:** Use heap allocation or global/static variables for thread arguments

### Thread Doesn't Start
- Check pthread_create() return value
- Ensure thread function signature is correct
- Verify thread ID variable is valid

### Threads Finish Before Main
If main exits before threads complete:
- Always use pthread_join() to wait
- Join all created threads
- Don't exit main prematurely

### Non-Deterministic Output
- Thread scheduling is non-deterministic
- Output order varies between runs
- This is normal and expected behavior
- Use synchronization if order matters

## Key Differences: Threads vs Processes

| Aspect | Threads | Processes |
|--------|---------|-----------|
| Memory | Shared address space | Separate address spaces |
| Creation | Faster (lightweight) | Slower (heavyweight) |
| Communication | Direct (shared memory) | IPC mechanisms needed |
| Overhead | Lower | Higher |
| Isolation | Less isolated | More isolated |
| Crashes | One thread crash affects all | Process crash isolated |

## Best Practices

1. **Always join threads** - Prevents resource leaks
2. **Check return values** - pthread functions return 0 on success
3. **Avoid global variables** - Or protect with synchronization
4. **Use proper types** - Don't ignore compiler warnings
5. **Keep thread functions simple** - Easier to debug and maintain
6. **Document thread interactions** - Critical for maintenance
7. **Test with different thread counts** - Expose race conditions

## Notes

- Thread creation is relatively inexpensive compared to process creation
- All threads in a process share the same file descriptors, signal handlers, and working directory
- Each thread has its own stack and thread-local storage
- The main thread is special - if it exits, the process exits
- `pthread_join()` blocks until the specified thread terminates
- Detached threads (pthread_detach) clean up automatically but can't be joined
- Thread IDs (pthread_t) are opaque types - don't assume they're integers
- Seeding random number generator should be done carefully in multi-threaded programs
- Thread scheduling is handled by the OS and is non-deterministic
- The commented versions provide detailed explanations of each concept
- These examples build foundation for understanding thread synchronization and advanced concurrency
