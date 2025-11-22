# Lecture 14 - Practical Threading and Synchronization

This directory contains practical threading examples demonstrating real-world applications and synchronization patterns for Fall 2025 Lecture 14.

## Contents

### Thread Review Examples
- **threadreview1.c** - Review of basic threading concepts
- **threadreview1commented.c** - Detailed explanation
- **threadreview2.c** - Review of argument passing
- **threadreview2commented.c** - Detailed explanation

### Agent Simulation
- **test1.c** - Multi-agent simulation with signals
- **test1commented.c** - Detailed explanation of agent state management

### Incremental Game Examples
- **threadIncremental.c** - Incremental game with threads and mutex
- **threadIncrementalcommented.c** - Detailed explanation
- **threadIncremental2.c** - Enhanced version
- **threadIncremental2commented.c** - Detailed explanation
- **threadIncremental3.c** - Advanced version with signals
- **threadIncremental3commented.c** - Detailed explanation
- **threadIncremental3_sigaction.c** - Using sigaction() instead of signal()
- **threadIncremental3_sigactioncommented.c** - Detailed explanation

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make test1
make threadIncremental
make threadIncremental3
```

**Note:** All programs require `-pthread` flag:
```bash
gcc threadIncremental.c -o threadIncremental -pthread
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Thread Review
```bash
./threadreview1
./threadreview2
```

**Expected behavior:**
- Reviews basic pthread concepts
- Demonstrates proper thread creation and joining
- Shows argument passing patterns

### Agent Simulation
```bash
./test1
# Let it run and watch agents change states
# Press Ctrl+C to signal all agents to stop
```

**Expected behavior:**
- Creates 5 agent threads with random states
- Each agent: eating, sleeping, moving, or vibing
- Agents randomly change states every 1-3 seconds
- Ctrl+C triggers graceful shutdown via signal handler
- All agents acknowledge shutdown and exit cleanly

**Key features:**
- Signal handler sets global flag
- Threads check flag in loop
- Demonstrates coordinated multi-thread shutdown
- Uses volatile sig_atomic_t for thread-safe flag

### Incremental Game (Basic)
```bash
./threadIncremental
```

**Input commands:**
- 1: Manually tick up income (+1)
- 2: Buy generator 1 (cost 10)
- 3: Buy generator 2 (cost 1000)
- 4: Buy generator 3 (cost 100000)

**Expected behavior:**
- Heartbeat thread updates display every second
- Generators automatically produce income:
  - Gen3 produces Gen2
  - Gen2 produces Gen1
  - Gen1 produces income
- Main thread handles user input
- Mutex protects all shared state
- Game continues indefinitely

**Example gameplay:**
1. Keep pressing '1' to get 10 income
2. Buy Gen1 (press 2)
3. Watch income automatically increase
4. Save up for Gen2 and Gen3

### Incremental Game (Enhanced)
```bash
./threadIncremental2
./threadIncremental3
```

**Expected behavior:**
- threadIncremental2: Enhanced UI and features
- threadIncremental3: Adds signal handling for graceful exit
- Press Ctrl+C to save and quit gracefully

### Incremental Game (with sigaction)
```bash
./threadIncremental3_sigaction
```

**Expected behavior:**
- Same as threadIncremental3
- Uses sigaction() for more robust signal handling
- Better portability and reliability

## Key Concepts Demonstrated

### Practical Threading Patterns

**Producer-Consumer Pattern:**
- Heartbeat thread produces updates
- Main thread consumes user input
- Mutex coordinates access to shared state

**Worker Pool Pattern:**
- Multiple agent threads work independently
- Each has own state but shares simulation flag
- Demonstrates parallel independent tasks

**Coordinated Shutdown:**
- Signal handler sets global flag
- All threads check flag and exit cleanly
- Main thread joins all workers before exit

### Mutex Usage Patterns

**Critical Section Protection:**
```c
pthread_mutex_lock(&mutex);
// Read and modify shared data
pthread_mutex_unlock(&mutex);
```

**Guidelines:**
- Lock before accessing shared data
- Keep critical sections short
- Unlock as soon as possible
- Always unlock (even on error paths)

**Common shared data:**
- Game state (income, generators)
- Simulation flags
- Counters and indices

### Thread Coordination

**Heartbeat Thread:**
- Runs continuously in background
- Performs periodic tasks (updates, display)
- Coordinates with main thread via mutex
- Never exits (or exits on shutdown signal)

**Worker Threads:**
- Perform specific tasks
- Check termination flag periodically
- Clean up and return when done
- Main thread joins to wait for completion

### Signal Integration with Threads

**Signal-safe shutdown:**
```c
volatile sig_atomic_t running = 1;

void handler(int sig) {
    running = 0;  // Atomic write
}

void* worker(void* arg) {
    while(running) {
        // Do work
    }
    return NULL;
}
```

**Key points:**
- Use volatile sig_atomic_t for flags
- Keep signal handler minimal
- Use write() not printf() in handlers
- All threads see flag change

### State Management

**Enum for states:**
```c
enum State {
    STATE_A,
    STATE_B,
    STATE_C
};
```

**Per-thread data:**
```c
struct ThreadData {
    int id;
    enum State state;
    // Other thread-specific fields
};
```

**Benefits:**
- Type safety
- Clear code
- Easy to extend

### Parallel Array Processing Pattern

**Chunk-based processing:**
1. Divide array into fixed-size chunks (e.g., 1000 elements)
2. Use shared counter to assign next chunk to thread
3. Mutex protects counter only (not array)
4. Threads process chunks in parallel
5. Continue until all chunks processed

**Pseudocode:**
```c
int nextChunk = 0;
pthread_mutex_t chunkMutex;

void* worker(void* arg) {
    while(1) {
        pthread_mutex_lock(&chunkMutex);
        int myChunk = nextChunk++;
        pthread_mutex_unlock(&chunkMutex);

        if(myChunk * CHUNK_SIZE >= ARRAY_SIZE)
            break;

        // Process chunk without lock
        processChunk(array, myChunk * CHUNK_SIZE);
    }
}
```

**Advantages:**
- Minimizes lock contention
- Good load balancing
- Scales with array size
- Fixed thread count

## Notes

- Mutex protects shared state, not individual variables conceptually
- Lock granularity matters: too coarse reduces parallelism, too fine causes overhead
- Thread-safe shutdown requires careful flag management
- Signal handlers in multi-threaded programs affect all threads
- Use sigaction() for production code (more reliable than signal())
- Heartbeat pattern useful for periodic updates/displays
- Agent pattern useful for independent simulations
- Always join threads to avoid resource leaks
- Consider using pthread_detach() for fire-and-forget threads
- Debugging multi-threaded programs is challenging (use print statements carefully)

## Troubleshooting

### Game display garbled:
- Ensure mutex protects all accesses to shared state
- Check that printMenu() is inside critical section
- Verify no printf() outside mutex when data can change

### Threads don't exit on Ctrl+C:
- Check signal handler sets the flag correctly
- Verify flag is volatile sig_atomic_t
- Ensure threads check flag frequently
- Make sure no infinite loops ignore flag

### Incremental game state inconsistent:
- Verify all reads and writes to income/generators are protected
- Check mutex is initialized before thread creation
- Ensure lock/unlock calls are balanced
- Look for missing locks around state access

### Simulation agents stuck:
- Check for deadlock (circular lock dependencies)
- Verify threads aren't waiting on each other
- Ensure signal handler doesn't block

### Performance issues:
- Profile critical section sizes
- Reduce time holding locks
- Consider lock-free algorithms for simple counters
- Avoid I/O inside critical sections when possible

### Race conditions still occur:
- Check all shared data is protected
- Verify same mutex is used for all accesses
- Look for time-of-check to time-of-use bugs
- Use ThreadSanitizer to detect races

## Assignment Hints

For the parallel array processing assignment:

1. **Fixed thread count** - Choose reasonable number (e.g., 4-10)
2. **Fixed chunk size** - Choose reasonable constant (e.g., 1000)
3. **Shared counter** - Protected by mutex
4. **Work distribution:**
   - Thread locks mutex
   - Gets next chunk index
   - Increments counter
   - Unlocks mutex
   - Processes chunk without holding lock
5. **Termination** - When chunk index exceeds array bounds
6. **Join all threads** - Wait for completion before reporting results

**Example structure:**
- Array size: 1,000,000 elements
- Chunk size: 1000 elements
- Number of threads: 8
- Total chunks: 1000
- Each thread processes ~125 chunks

## Resources

- Review Lecture 11 for mutex basics
- Review Lecture 13 for signals
- Chapter 13 of "Dive into Systems" - Threading and synchronization
- `man pthread_create`, `man pthread_mutex_init`
- ThreadSanitizer: `gcc -fsanitize=thread`
