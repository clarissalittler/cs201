# Deadlock and Synchronization

This directory contains examples demonstrating deadlocks, mutexes, and semaphores in concurrent programming with pthreads.

## Contents

### Deadlock Examples
- **dead1.c** - Classic deadlock scenario with two threads acquiring locks in opposite order
- **philo1.c** - Dining philosophers problem (classic deadlock scenario)
- **philo2.c** - Dining philosophers with deadlock prevention attempt
- **philo3.c** - Dining philosophers with different deadlock solution
- **philo4.c** - Dining philosophers with another approach
- **philo5.c** - Dining philosophers final solution

### Mutex Examples
- **mut1.c** - Basic mutex usage with 1000 threads incrementing a counter
- **mut2.c** - Another mutex example

### Semaphore Examples
- **sem1.c** - Basic semaphore usage
- **sem2.c** - Advanced semaphore patterns
- **mutSem.c** - Combining mutexes and semaphores

## Compilation

### Compile all programs:
```bash
make
```

### Compile individual programs:
```bash
gcc -pthread dead1.c -o dead1
gcc -pthread philo1.c -o philo1
gcc -pthread mut1.c -o mut1
gcc -pthread sem1.c -o sem1
```

### Clean up:
```bash
make clean
```

**Note:** Always use the `-pthread` flag when compiling programs that use pthreads.

## Running the Examples

### Deadlock Demonstration (dead1.c)

This demonstrates a classic deadlock scenario.

```bash
./dead1
```

**Expected behavior:**
- Thread 1 acquires lock 1, waits for lock 2
- Thread 2 acquires lock 2, waits for lock 1
- **DEADLOCK**: Program hangs indefinitely
- Use Ctrl+C to terminate

**Key concept:** Circular wait - each thread holds a resource the other needs.

### Dining Philosophers Problem

The classic concurrency problem demonstrating deadlock and resource contention.

**philo1.c** - Basic version (demonstrates deadlock):
```bash
./philo1
```

**Expected behavior:**
- 5 philosophers try to eat using 5 utensils (forks)
- Each philosopher needs 2 utensils to eat
- All philosophers pick up left utensil, none can get right
- **DEADLOCK**: Program hangs
- Use Ctrl+C to terminate

**philo2-philo5** - Various deadlock prevention strategies:
```bash
./philo2  # Try different prevention approach
./philo3  # Another solution
./philo4  # Yet another approach
./philo5  # Final optimized solution
```

**Expected behavior:**
- Should not deadlock
- Philosophers alternate between thinking and eating
- Run for 10-30 seconds to observe behavior
- Use Ctrl+C to stop

### Mutex Protection (mut1.c)

This demonstrates proper mutex usage to protect shared data.

```bash
./mut1
```

**Expected behavior:**
- 1000 threads each increment counter 100,000 times
- Final value: 100,000,000 (correct with mutex protection)
- Without mutex, value would be incorrect due to race conditions

**Note:** This may take several seconds to complete.

### Semaphore Examples

```bash
./sem1
./sem2
./mutSem
```

## Key Concepts Demonstrated

### Deadlock Conditions

A deadlock occurs when all four conditions are met:

1. **Mutual Exclusion**: Resources cannot be shared
2. **Hold and Wait**: Threads hold resources while waiting for others
3. **No Preemption**: Resources cannot be forcibly taken
4. **Circular Wait**: Circular chain of threads waiting for resources

**Prevention:** Break any one of these conditions.

### Deadlock Prevention Strategies

**1. Lock Ordering** (breaks circular wait):
```c
// Always acquire locks in same order
pthread_mutex_lock(&lock1);
pthread_mutex_lock(&lock2);
// ...
pthread_mutex_unlock(&lock2);
pthread_mutex_unlock(&lock1);
```

**2. Timeout with Trylock** (breaks hold and wait):
```c
while (pthread_mutex_trylock(&lock2) != 0) {
    pthread_mutex_unlock(&lock1);
    usleep(random_time);
    pthread_mutex_lock(&lock1);
}
```

**3. Resource Hierarchy** (enforces ordering):
- Assign numbers to resources
- Always acquire lower-numbered resources first

**4. Limit Resources** (prevents all from being held):
- Dining philosophers: Only allow 4 of 5 to eat simultaneously

### Mutex (Mutual Exclusion)

Protects critical sections from concurrent access:

```c
pthread_mutex_t lock;
pthread_mutex_init(&lock, NULL);

// Critical section
pthread_mutex_lock(&lock);
shared_variable++;  // Protected operation
pthread_mutex_unlock(&lock);

pthread_mutex_destroy(&lock);
```

### Semaphores

Generalized synchronization primitive:

```c
#include <semaphore.h>

sem_t sem;
sem_init(&sem, 0, 1);  // Initial value 1 (binary semaphore)

sem_wait(&sem);   // Decrement (P operation)
// Critical section
sem_post(&sem);   // Increment (V operation)

sem_destroy(&sem);
```

**Binary Semaphore (0/1):** Similar to mutex
**Counting Semaphore (0-N):** Limits concurrent access to N resources

## Debugging Deadlocks

### Detect Deadlock at Runtime

**Using timeout with pthread_mutex_timedlock():**
```c
struct timespec timeout;
clock_gettime(CLOCK_REALTIME, &timeout);
timeout.tv_sec += 5;  // 5 second timeout

if (pthread_mutex_timedlock(&lock, &timeout) != 0) {
    printf("Deadlock detected!\n");
}
```

### Find Deadlocks with Tools

**GDB (GNU Debugger):**
```bash
gcc -g -pthread dead1.c -o dead1
gdb ./dead1
(gdb) run
# Wait for deadlock
(gdb) thread apply all bt  # Show all thread backtraces
```

**Helgrind (Valgrind tool):**
```bash
valgrind --tool=helgrind ./dead1
```

**ThreadSanitizer:**
```bash
gcc -fsanitize=thread -g -pthread dead1.c -o dead1
./dead1
```

## Common Pitfalls

### 1. Forgetting to Unlock
```c
pthread_mutex_lock(&lock);
if (error_condition) return;  // FORGOT TO UNLOCK!
pthread_mutex_unlock(&lock);
```

**Solution:** Use cleanup handlers or careful error handling.

### 2. Locking Same Mutex Twice
```c
pthread_mutex_lock(&lock);
pthread_mutex_lock(&lock);  // DEADLOCK with self!
```

**Solution:** Use recursive mutexes or avoid nested locking.

### 3. Inconsistent Lock Ordering
```c
// Thread 1
lock(A); lock(B);

// Thread 2
lock(B); lock(A);  // POTENTIAL DEADLOCK!
```

**Solution:** Always acquire locks in same order.

### 4. Holding Locks Too Long
```c
pthread_mutex_lock(&lock);
expensive_computation();  // BAD: Holds lock during long operation
pthread_mutex_unlock(&lock);
```

**Solution:** Minimize critical section duration.

## Troubleshooting

### Program hangs indefinitely
- Likely deadlock - use Ctrl+C to terminate
- Run with debugger or Helgrind to identify deadlock location
- Check lock acquisition order in all threads

### Incorrect final values in mutex examples
- Ensure `-pthread` flag is used during compilation
- Verify mutexes are initialized before use
- Check that all critical sections are protected

### Segmentation fault
- Ensure mutexes/semaphores are initialized before use
- Don't destroy mutexes while threads are using them
- Verify all threads are joined before cleanup

## Performance Considerations

### Fine-grained vs Coarse-grained Locking

**Coarse-grained (one big lock):**
- Simple, less prone to deadlock
- Lower parallelism, more contention

**Fine-grained (many small locks):**
- Higher parallelism
- Complex, more deadlock-prone
- Better performance when done correctly

### Lock Contention

High contention reduces performance:
- Minimize time holding locks
- Use read-write locks for read-heavy workloads
- Consider lock-free data structures for hot paths

## See Also

- `concurrency-tutorial/synchronization-examples/` - More synchronization patterns
- `concurrency-tutorial/commented-examples/04-thread-synchronization/` - Detailed explanations
- Main repository README for additional resources

## Notes

- All examples use POSIX threads (pthreads)
- Deadlock examples are intentionally broken to demonstrate the problem
- Non-deadlock examples show various prevention strategies
- Always compile with `-pthread` flag
- Some examples run indefinitely - use Ctrl+C to stop
