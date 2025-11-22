# Lecture 10 - Condition Variables and Advanced Thread Synchronization

This directory contains examples for Winter 2025 Lecture 10, demonstrating condition variables, producer-consumer patterns, and advanced threading synchronization techniques.

## Contents

### Mutex Review
- **mutexreview.c** - Review of mutex operations
- **mutexreviewcommented.c** - Detailed mutex explanation

### Condition Variables
- **condtest.c** - Producer-consumer with condition variables
- **condtestcommented.c** - Complete explanation of condition variable usage
- **condbespoke1.c** - Custom condition variable implementation
- **condbespoke1commented.c** - Building synchronization primitives from scratch

## Compilation

### Compile all examples:
```bash
make
```

**Note:** Must link with pthread library using `-pthread` flag.

### Compile specific programs:
```bash
make condtest
make mutexreview
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Mutex Review
```bash
./mutexreview
```
**Expected behavior:**
- Demonstrates proper mutex usage
- Shows thread-safe counter incrementation
- Verifies correct synchronization

### Producer-Consumer with Condition Variables
```bash
./condtest
```
**Expected behavior:**
- Producer thread creates items
- Consumer thread processes items
- Condition variables coordinate when buffer is full/empty
- Demonstrates thread communication pattern

### Custom Condition Variable
```bash
./condbespoke1
```
**Expected behavior:**
- Shows how to build synchronization from basic primitives
- Demonstrates understanding of underlying mechanisms

## Key Concepts Demonstrated

### Condition Variables

**Purpose:** Allow threads to wait for specific conditions

**Problem with just mutexes:**
```c
// BAD: Busy waiting (wastes CPU)
while (!data_ready) {
    pthread_mutex_unlock(&lock);
    // Brief sleep?
    pthread_mutex_lock(&lock);
}
```

**Solution: Condition Variables**
```c
pthread_cond_t cond;
pthread_mutex_t lock;

// Initialize
pthread_cond_init(&cond, NULL);
pthread_mutex_init(&lock, NULL);
```

### Condition Variable Operations

#### pthread_cond_wait()
```c
pthread_mutex_lock(&lock);

while (!condition_met) {
    // Atomically: unlock mutex and wait
    // When signaled: relock mutex and return
    pthread_cond_wait(&cond, &lock);
}

// Condition is now true and mutex is locked
// Process data...

pthread_mutex_unlock(&lock);
```

**What pthread_cond_wait() does:**
1. Atomically unlocks mutex
2. Puts thread to sleep
3. Waits for signal
4. When signaled, relocks mutex
5. Returns

#### pthread_cond_signal()
```c
pthread_mutex_lock(&lock);

// Change shared state
data_ready = 1;

// Wake up ONE waiting thread
pthread_cond_signal(&cond);

pthread_mutex_unlock(&lock);
```

#### pthread_cond_broadcast()
```c
pthread_mutex_lock(&lock);

// Change shared state
all_done = 1;

// Wake up ALL waiting threads
pthread_cond_broadcast(&cond);

pthread_mutex_unlock(&lock);
```

### Producer-Consumer Pattern

**Classic synchronization problem:**
- Producer creates items, puts in buffer
- Consumer takes items from buffer, processes them
- Buffer has limited size
- Need to coordinate when buffer is full/empty

**Implementation:**
```c
#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int count = 0;  // Items in buffer

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void* producer(void* arg) {
    for (int i = 0; i < 100; i++) {
        int item = produce_item();

        pthread_mutex_lock(&lock);

        // Wait while buffer is full
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&not_full, &lock);
        }

        // Add item to buffer
        buffer[count++] = item;

        // Signal consumer
        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < 100; i++) {
        pthread_mutex_lock(&lock);

        // Wait while buffer is empty
        while (count == 0) {
            pthread_cond_wait(&not_empty, &lock);
        }

        // Remove item from buffer
        int item = buffer[--count];

        // Signal producer
        pthread_cond_signal(&not_full);

        pthread_mutex_unlock(&lock);

        consume_item(item);
    }
    return NULL;
}
```

### Why Use `while` Instead of `if`?

**Always use while loop with condition variables:**

```c
// CORRECT:
while (!condition) {
    pthread_cond_wait(&cond, &lock);
}

// WRONG:
if (!condition) {
    pthread_cond_wait(&cond, &lock);
}
```

**Reasons:**
1. **Spurious wakeups**: Thread may wake without signal
2. **Multiple consumers**: Another thread might consume the item
3. **Signal vs Broadcast**: Multiple threads may wake up
4. **Condition may change**: Another thread might change state

### Semaphores

**Alternative synchronization primitive:**
- Like a mutex, but can have count > 1
- Controls access to resource with limited instances
- Can synchronize across processes (unlike mutexes)

**Basic operations:**
```c
#include <semaphore.h>

sem_t semaphore;

// Initialize with count
sem_init(&semaphore, 0, 5);  // Allow 5 concurrent accesses

// Wait (decrement)
sem_wait(&semaphore);
// Access resource
sem_post(&semaphore);

// Cleanup
sem_destroy(&semaphore);
```

### Semaphore vs Mutex

**Mutex (Binary Semaphore):**
- Only 0 or 1
- Ownership (locker must unlock)
- Protects critical section

**Counting Semaphore:**
- Can be > 1
- No ownership (any thread can post)
- Controls resource access count

### mmap for Shared Memory

**Required for inter-process semaphores:**

```c
#include <sys/mman.h>

// Create shared memory
void* shared = mmap(NULL, sizeof(sem_t),
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS,
                    -1, 0);

sem_t* sem = (sem_t*)shared;
sem_init(sem, 1, 1);  // 1 = shared between processes
```

### Condition Variable Operations Summary

- **pthread_cond_init()**: Initialize condition variable
- **pthread_cond_destroy()**: Clean up condition variable
- **pthread_cond_wait()**: Wait for condition (with mutex)
- **pthread_cond_timedwait()**: Wait with timeout
- **pthread_cond_signal()**: Wake one waiting thread
- **pthread_cond_broadcast()**: Wake all waiting threads

### Best Practices

#### Always Hold Mutex When Using Condition Variables
```c
pthread_mutex_lock(&lock);
pthread_cond_wait(&cond, &lock);  // Requires locked mutex
pthread_mutex_unlock(&lock);
```

#### Check Condition in While Loop
```c
while (!ready) {
    pthread_cond_wait(&cond, &lock);
}
```

#### Signal After Changing Condition
```c
pthread_mutex_lock(&lock);
ready = 1;
pthread_cond_signal(&cond);
pthread_mutex_unlock(&lock);
```

## Common Patterns

### One Producer, One Consumer
- Use two condition variables: not_full, not_empty
- Producer waits on not_full, signals not_empty
- Consumer waits on not_empty, signals not_full

### Multiple Producers, Multiple Consumers
- Same as above but use broadcast for multiple waiters
- Or use signal if any one thread can handle the work

### Barrier Synchronization
- All threads wait until N threads arrive
- Last thread broadcasts to wake everyone

### Read-Write Locks
- Multiple readers OR one writer
- Build using mutexes and condition variables

## Troubleshooting

### Deadlock?
- Check all mutexes are unlocked
- Verify no circular wait conditions
- Ensure signal happens after wait

### Threads not waking up?
- Verify pthread_cond_signal() is called
- Check condition is actually changed before signal
- Ensure mutex is held when signaling

### Spurious wakeups causing bugs?
- Always use while loop, not if
- Re-check condition after waking

### Poor performance?
- Minimize critical section size
- Use signal instead of broadcast when possible
- Consider lock-free data structures

## Performance Considerations

**Condition variables are efficient:**
- No busy waiting (CPU efficient)
- Kernel handles thread scheduling
- Minimal overhead when properly used

**Tips:**
- Keep critical sections short
- Don't hold locks while doing I/O
- Use appropriate granularity of locking

## Notes

- Condition variables require mutex for correctness
- Always use while loop to check condition
- signal() wakes one thread, broadcast() wakes all
- Semaphores can work across processes
- mmap creates shared memory regions
- Understanding synchronization is crucial for concurrent programming
- Producer-consumer is a fundamental pattern in concurrent systems
- These primitives are building blocks for complex synchronization
