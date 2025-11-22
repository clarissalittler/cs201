# Synchronization Exercises

This section focuses on critical synchronization concepts: preventing race conditions, coordinating threads, and avoiding deadlocks.

## Learning Objectives

After completing these exercises, you will be able to:
- Identify and demonstrate race conditions in multithreaded programs
- Use mutexes to protect shared resources and prevent data corruption
- Implement producer-consumer patterns using condition variables
- Understand and prevent deadlock situations
- Implement advanced synchronization patterns like barriers

## Prerequisites

- Completion of thread introduction exercises
- Understanding of thread creation and management
- Familiarity with shared memory issues in multithreaded programs
- Basic understanding of critical sections

## Exercises

### Exercise 1: Race Condition Demonstration 🟢
**File**: `exercise1.c`
**Concepts**: Race conditions, non-atomic operations, lost updates

Create a program where multiple threads increment a shared counter without synchronization. Observe and measure the "lost updates" problem to understand why synchronization is necessary.

### Exercise 2: Mutex-Protected Counter 🟢
**File**: `exercise2.c`
**Concepts**: Mutexes, critical sections, atomic operations

Fix the race condition from Exercise 1 by using a mutex to protect the shared counter. Learn proper mutex initialization, locking, and unlocking patterns.

### Exercise 3: Producer-Consumer Queue 🟡
**File**: `exercise3.c`
**Concepts**: Condition variables, producer-consumer pattern, thread coordination

Implement a bounded buffer where producer threads add items and consumer threads remove them. Use mutexes and condition variables for proper synchronization and thread signaling.

### Exercise 4: Dining Philosophers 🔴
**File**: `exercise4.c`
**Concepts**: Deadlock prevention, resource ordering, circular wait

Implement the classic dining philosophers problem with deadlock prevention. Five philosophers must share five forks without causing deadlock or starvation.

### Exercise 5: Barrier Synchronization 🔴
**File**: `exercise5.c`
**Concepts**: Barriers, phase synchronization, reusable barriers

Create a reusable barrier that synchronizes multiple threads at specific points. All threads must reach the barrier before any can proceed to the next phase.

## Key Synchronization Primitives

| Primitive | Purpose | Use Case |
|-----------|---------|----------|
| Mutex | Mutual exclusion | Protecting shared data |
| Condition Variable | Thread signaling | Waiting for conditions |
| Semaphore | Counting resources | Limited resource pools |
| Barrier | Phase synchronization | Coordinating computation stages |

## Common Synchronization Patterns

### Critical Section Pattern
```c
pthread_mutex_lock(&mutex);
// Critical section - access shared data
pthread_mutex_unlock(&mutex);
```

### Condition Variable Pattern
```c
pthread_mutex_lock(&mutex);
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
// Condition is true, proceed
pthread_mutex_unlock(&mutex);
```

### Signal Pattern
```c
pthread_mutex_lock(&mutex);
// Modify shared state
condition = true;
pthread_cond_signal(&cond);  // or pthread_cond_broadcast()
pthread_mutex_unlock(&mutex);
```

## Compilation

```bash
gcc -o exercise1 exercise1.c -pthread
gcc -o exercise2 exercise2.c -pthread
gcc -o exercise3 exercise3.c -pthread
gcc -o exercise4 exercise4.c -pthread
gcc -o exercise5 exercise5.c -pthread
```

**Note**: The `-pthread` flag is essential for linking pthread library and enabling proper synchronization.

## Tips

- **Always** initialize mutexes before use: `pthread_mutex_init()`
- **Always** destroy mutexes when done: `pthread_mutex_destroy()`
- Keep critical sections as small as possible
- Use `pthread_cond_wait()` in a while loop, not an if statement
- Unlock mutexes in the same function/thread that locked them
- Be careful with lock ordering to prevent deadlock
- Use `pthread_mutex_trylock()` for non-blocking attempts
- Always check return values of pthread functions

## Common Pitfalls to Avoid

1. **Forgetting to unlock** - Always unlock mutexes, even on error paths
2. **Wrong unlock order** - Unlock in reverse order of locking when using multiple mutexes
3. **Condition variable without mutex** - Always hold mutex when using condition variables
4. **Using if instead of while** - Condition variables can have spurious wakeups
5. **Deadlock from circular waiting** - Establish a global lock ordering
6. **Race conditions on initialization** - Initialize synchronization primitives before creating threads

## Debugging Synchronization Issues

- Use `-g` flag for debugging symbols
- Run with tools like `valgrind --tool=helgrind` to detect race conditions
- Add print statements with thread IDs to trace execution
- Use `sleep()` or `usleep()` strategically to make race conditions more visible
- Test with different numbers of threads and iterations
