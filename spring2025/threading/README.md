# Threading and Concurrency

This directory contains examples demonstrating both fork-based and thread-based concurrency for Spring 2025, including race conditions and synchronization mechanisms.

## Contents

### Fork Examples
- **forkVar.c** - Demonstrates variable behavior after fork (copy-on-write)
- **forkVarcommented.c** - Commented version explaining memory independence
- **fork1.c** - Simple fork creating parent and child processes
- **fork1commented.c** - Detailed explanation of basic fork mechanics
- **fork2.c** - Fork with parent/child differentiation
- **fork2commented.c** - Explains how to identify parent vs child
- **fork3.c** - Fork with wait synchronization
- **fork3commented.c** - Shows proper process synchronization
- **fork4.c** - Multiple fork pattern (creates process tree)
- **fork4commented.c** - Explains exponential process creation
- **fork5.c** - Fork with exit status handling
- **fork5commented.c** - Demonstrates proper wait() usage

### Thread Examples
- **thread1.c** - Basic pthread creation and joining
- **thread1commented.c** - Detailed explanation of pthread basics
- **threadResult.c** - Threads with return values
- **threadResultcommented.c** - Shows pthread_join() with result retrieval

### Race Condition Examples
- **threadRace1.c** - Demonstrates race condition with shared counter
- **threadRace1commented.c** - Explains why race conditions occur

### Synchronization Examples
- **threadmutex.c** - Fixes race condition using mutex
- **threadmutexcommented.c** - Detailed mutex usage explanation

## Compilation

### Compile all programs:
```bash
# Fork examples (no special flags needed)
gcc fork1.c -o fork1
gcc fork2.c -o fork2
gcc fork3.c -o fork3
gcc fork4.c -o fork4
gcc fork5.c -o fork5
gcc forkVar.c -o forkVar

# Thread examples (need -pthread flag)
gcc thread1.c -o thread1 -pthread
gcc threadResult.c -o threadResult -pthread
gcc threadRace1.c -o threadRace1 -pthread
gcc threadmutex.c -o threadmutex -pthread
```

### Compile with debugging symbols:
```bash
gcc -Wall -g thread1.c -o thread1 -pthread
```

### Compile all at once:
```bash
# Fork programs
for f in fork1 fork2 fork3 fork4 fork5 forkVar; do
  gcc ${f}.c -o ${f}
done

# Thread programs
for f in thread1 threadResult threadRace1 threadmutex; do
  gcc ${f}.c -o ${f} -pthread
done
```

## Running the Examples

### Fork Examples

**fork1 - Basic Fork:**
```bash
./fork1
```
**Expected behavior:**
- Prints message twice (once from parent, once from child)
- Demonstrates that fork creates a complete copy

**fork2 - Parent/Child Identification:**
```bash
./fork2
```
**Expected behavior:**
- Parent knows child's PID
- Child receives 0 from fork()
- Different messages from parent and child

**fork3 - Synchronization with wait():**
```bash
./fork3
```
**Expected behavior:**
- Parent waits for child to complete
- Orderly output showing proper synchronization
- No zombie processes

**fork4 - Process Tree:**
```bash
./fork4
```
**Expected behavior:**
- Multiple forks create exponential process tree
- Demonstrates how N forks create 2^N processes
- Shows process relationships

**fork5 - Exit Status:**
```bash
./fork5
```
**Expected behavior:**
- Child exits with specific status code
- Parent retrieves and displays exit status
- Demonstrates WEXITSTATUS() macro usage

**forkVar - Variable Independence:**
```bash
./forkVar
```
**Expected behavior:**
- Shows that child and parent have separate memory
- Changes in child don't affect parent variables
- Demonstrates copy-on-write behavior

### Thread Examples

**thread1 - Basic Threading:**
```bash
./thread1
```
**Expected behavior:**
- Creates two threads
- Each thread prints its message
- Main thread waits for both to complete
- Output may be interleaved (non-deterministic order)

**Expected output (order may vary):**
```
Hi there I'm one thread
Hi there I'm a different thread
```

**threadResult - Thread Return Values:**
```bash
./threadResult
```
**Expected behavior:**
- Thread performs computation
- Returns result via pthread_join()
- Main thread receives and displays result

### Race Condition Demonstration

**threadRace1 - Unprotected Shared Variable:**
```bash
./threadRace1
```
**Expected behavior:**
- Multiple threads increment shared counter
- Final count is LESS than expected due to race condition
- Demonstrates need for synchronization

**Run multiple times to see variation:**
```bash
for i in {1..5}; do ./threadRace1; done
```

**Why race condition occurs:**
- Multiple threads read-modify-write same variable
- Operations are not atomic
- Intermediate results get lost

### Synchronization Solution

**threadmutex - Mutex Protection:**
```bash
./threadmutex
```
**Expected behavior:**
- Multiple threads increment shared counter with mutex
- Final count is CORRECT (matches expected value)
- Demonstrates proper synchronization

**Compare with race condition:**
```bash
echo "Without mutex:"
./threadRace1
echo "With mutex:"
./threadmutex
```

## Key Concepts Demonstrated

### Fork vs Threads

**Processes (fork):**
- Completely separate memory spaces
- Higher creation overhead
- Better isolation (crashes don't affect parent)
- Communication requires IPC mechanisms
- Traditional Unix concurrency model

**Threads (pthread):**
- Share memory space (same variables)
- Lower creation overhead
- Less isolation (shared state can cause bugs)
- Easy communication via shared memory
- Requires synchronization for shared data
- Modern concurrency model

### Race Conditions

**What is a race condition?**
- Multiple threads access shared data
- At least one thread modifies the data
- Operations are not synchronized
- Result depends on timing/scheduling

**Example (threadRace1):**
```c
counter++;  // Not atomic! Actually three operations:
// 1. Load counter into register
// 2. Increment register
// 3. Store register back to counter
// Another thread can interfere between any of these steps
```

### Mutex (Mutual Exclusion)

**How mutexes work:**
- Only one thread can hold mutex at a time
- Other threads block (wait) until mutex is available
- Protects critical sections of code

**Usage pattern:**
```c
pthread_mutex_lock(&mutex);    // Acquire lock
// Critical section - only one thread at a time
counter++;
pthread_mutex_unlock(&mutex);  // Release lock
```

### Thread Creation and Management

**pthread_create():**
- Creates new thread
- Requires thread ID variable, attributes, function, and argument
- Thread starts executing immediately

**pthread_join():**
- Waits for thread to complete
- Retrieves return value
- Prevents zombie threads
- Similar to wait() for processes

### Process Synchronization

**wait() and waitpid():**
- Parent waits for child process to terminate
- Prevents zombie processes
- Retrieves exit status
- Essential for proper process cleanup

## Troubleshooting

### Race Condition Debugging
```bash
# Run program multiple times to see inconsistent results
for i in {1..10}; do ./threadRace1 | grep "Final"; done

# Use thread sanitizer to detect race conditions
gcc -fsanitize=thread threadRace1.c -o threadRace1 -pthread
./threadRace1
```

### Deadlock Prevention
```bash
# If program hangs:
# 1. Check for circular mutex dependencies
# 2. Ensure all lock() calls have matching unlock()
# 3. Use timeout mechanisms
# 4. Always lock mutexes in same order
```

### Segmentation Faults
```bash
# Common causes with threads:
# 1. Passing stack variables to threads (they disappear!)
# 2. Not initializing mutex before use
# 3. Accessing freed memory from other thread

# Debug with gdb and threads
gdb ./thread1
(gdb) run
(gdb) info threads
(gdb) thread <n>  # Switch to thread n
(gdb) bt         # Backtrace for current thread
```

### Compilation Errors
```bash
# If you see "undefined reference to pthread_create":
# Missing -pthread flag
gcc threadRace1.c -o threadRace1 -pthread  # Correct

# If you see warnings about unused variables in commented versions:
gcc -Wno-unused-variable thread1commented.c -o thread1commented -pthread
```

## Notes

- Thread execution order is non-deterministic (depends on OS scheduler)
- Always use -pthread flag when compiling threaded programs
- Commented versions provide extensive explanations for learning
- Race conditions are subtle and may not appear on every run
- Mutexes have performance overhead but are essential for correctness
- Fork is heavyweight, threads are lightweight
- Modern applications prefer threads over fork for concurrency
- Always join threads or processes to avoid zombies
- Shared memory makes threads fast but requires careful synchronization
- The Agenda.org file references additional examples throughout the repository

## Assignment 3 Context

The Agenda.org mentions Assignment 3, which involves:
- Creating a thread pool with configurable size
- Distributing array work in chunks
- Each thread claims chunks dynamically
- Using mutex for work coordination
- Processing large arrays (millions of elements)
- Threads exit when no work remains

This assignment demonstrates:
- Work distribution patterns
- Dynamic task allocation
- Thread pool management
- Practical mutex usage

## Related Topics

See also:
- `/home/user/cs201/spring2025/forking/` - Fork-based IPC and process management
- `/home/user/cs201/concurrency-tutorial/` - More advanced concurrency examples
- `/home/user/cs201/winter2025/lecture9/` - Additional lecture examples
- `/home/user/cs201/winter2025/lecture13/` - Advanced IPC mechanisms
