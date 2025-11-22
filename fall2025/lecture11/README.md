# Lecture 11 - Advanced Threading and Synchronization

This directory contains examples demonstrating advanced threading concepts, race conditions, and synchronization for Fall 2025 Lecture 11.

## Contents

### Function Pointers
- **funcpoint1.c** - Higher-order functions (map) with function pointers
- **funcpoint1commented.c** - Detailed explanation of function pointers
- **funcpoint2.c** - More complex function pointer examples
- **funcpoint2commented.c** - Detailed explanation

### Void Pointer Arithmetic
- **voidArith.c** - Void pointer casting and arithmetic
- **voidArithcommented.c** - Detailed explanation of void* usage

### Thread Examples
- **threads1.c** - Basic thread creation and joining
- **threads1commented.c** - Detailed explanation
- **threads2.c** - Passing arguments to threads
- **threads2commented.c** - Detailed explanation
- **threads3.c** - Returning data from threads
- **threads3commented.c** - Detailed explanation

### Race Conditions
- **threadRace.c** - Demonstrating race condition with shared counter
- **threadRacecommented.c** - Detailed explanation of race condition
- **threadRace2.c** - Race condition with more complex scenario
- **threadRace2commented.c** - Detailed explanation
- **threadRace3.c** - Fixed version using mutex
- **threadRace3commented.c** - Detailed explanation of mutex usage

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make funcpoint1
make threadRace
make threadRace3
```

**Note:** All thread programs require the `-pthread` flag:
```bash
gcc threadRace.c -o threadRace -pthread
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Function Pointers - Map
```bash
./funcpoint1
```

**Expected behavior:**
- Prints initial array: [0,1,2,3,4]
- Applies incWrongly function to each element (adds 2)
- Prints result: [2,3,4,5,6]
- Demonstrates higher-order functions in C

### Function Pointers - Advanced
```bash
./funcpoint2
```

**Expected behavior:**
- Shows more complex function pointer usage
- Demonstrates function pointers as return values
- Shows function pointer arrays

### Void Pointer Arithmetic
```bash
./voidArith
```

**Expected behavior:**
- Demonstrates casting void* to different types
- Shows how to do arithmetic with void pointers
- Illustrates why void* is important for generic code

### Basic Threads
```bash
./threads1
./threads2
./threads3
```

**Expected behavior:**
- threads1: Creates and joins threads
- threads2: Passes data to thread via argument
- threads3: Returns data from thread via return value

### Race Condition (BUGGY)
```bash
./threadRace
```

**Expected behavior (incorrect):**
- Creates 100 threads that each increment shared counter
- **Expected:** counter = 100
- **Actual:** counter is usually much less (often 1-10)
- Output varies each run (non-deterministic)
- Demonstrates classic race condition

**Why it fails:**
1. Thread A reads counter (0)
2. Thread B reads counter (0)
3. Thread A increments and writes (1)
4. Thread B increments and writes (1)
5. Result: counter = 1, but should be 2!

### Race Condition Variations
```bash
./threadRace2
```

**Expected behavior:**
- More complex race condition scenario
- Shows how race conditions occur in realistic code
- Results are unpredictable

### Fixed with Mutex (CORRECT)
```bash
./threadRace3
```

**Expected behavior:**
- Creates 100 threads that each increment shared counter
- Uses mutex to protect critical section
- **Result:** counter = 100 (correct!)
- Output is deterministic and correct
- May be slightly slower due to synchronization overhead

## Key Concepts Demonstrated

### Function Pointers
- **Declaration:** `return_type (*pointer_name)(arg_types)`
- **Example:** `int (*func)(int)` - pointer to function taking int, returning int
- **Usage:** Call through pointer: `result = func(5)`
- **Higher-order functions:** Functions that take functions as arguments
- **Use cases:** Callbacks, generic algorithms (map, fold, filter)

### Void Pointers
- **void\*** - Generic pointer type
- Can point to any data type
- **Cannot dereference directly** - must cast first
- **No pointer arithmetic** on void* (must cast to sized type)
- Used for generic interfaces (malloc, thread arguments)

### Casting to/from void*
```c
int x = 42;
void* ptr = &x;           // Implicit cast to void*
int* int_ptr = (int*)ptr; // Explicit cast from void*
int value = *int_ptr;     // Now can dereference
```

### Thread Arguments and Return Values
- **Passing data in:** Use 4th argument of pthread_create
  - Cast to void*: `pthread_create(&t, NULL, func, (void*)&data)`
  - Cast back in function: `int* ptr = (int*)arg`
- **Returning data:** Return void* from thread function
  - Retrieve with pthread_join: `pthread_join(t, &result)`

### Race Conditions
- **Definition:** Bug where program behavior depends on timing
- **Cause:** Multiple threads accessing shared data without synchronization
- **Symptoms:**
  - Non-deterministic results
  - Works sometimes, fails other times
  - Hard to debug (Heisenbugs)
- **Critical section:** Code that accesses shared resource

### Mutexes (Mutual Exclusion)
- **pthread_mutex_t** - Mutex type
- **pthread_mutex_init()** - Initialize mutex
- **pthread_mutex_lock()** - Acquire lock (blocks if held)
- **pthread_mutex_unlock()** - Release lock
- **pthread_mutex_destroy()** - Clean up mutex

### Mutex Pattern
```c
pthread_mutex_t lock;
pthread_mutex_init(&lock, NULL);

// In critical section:
pthread_mutex_lock(&lock);
// ... access shared data ...
pthread_mutex_unlock(&lock);

pthread_mutex_destroy(&lock);
```

### Critical Section
- Code that accesses shared mutable data
- Must be protected by synchronization primitive
- Keep as short as possible (reduce contention)
- Never hold multiple locks without careful ordering

## Notes

- Function pointers enable callbacks and generic programming
- void* is C's way of doing generic programming
- Thread arguments/returns must use void* pointers
- Race conditions are non-deterministic bugs (hardest to debug)
- **Always** protect shared mutable data with synchronization
- Mutexes ensure only one thread at a time in critical section
- Forgetting to unlock mutex causes deadlock
- Lock/unlock must be balanced (use goto cleanup or RAII in C++)
- Minimize critical section size for better performance
- Race conditions often don't appear in testing (timing dependent)
- Use tools like ThreadSanitizer to detect race conditions
- Mutexes have overhead - don't use for every single variable

## Troubleshooting

### Race condition not appearing:
- Race conditions are timing-dependent
- May not appear with small number of threads
- Add sleep() or work in critical section to make more likely
- Run multiple times - results should vary
- Use debugging tools (valgrind --tool=helgrind, ThreadSanitizer)

### Mutex doesn't fix race condition:
- Verify all accesses to shared data are protected
- Check lock/unlock pairs are balanced
- Ensure same mutex is used for all accesses
- Verify mutex is initialized before use

### Program hangs (deadlock):
- Check every lock has corresponding unlock
- Verify unlock is called even on error paths
- Check for holding lock too long
- Look for circular lock dependencies

### Compilation errors with function pointers:
- Check pointer declaration syntax carefully
- Ensure function signature matches pointer type
- Remember parentheses: `int (*ptr)(int)` not `int *ptr(int)`

### Segfault with void* casting:
- Verify pointer is not NULL before dereferencing
- Check cast is to correct type
- Ensure pointed-to data is still valid (not out of scope)
- Be careful with stack variables and thread lifetimes

## Resources

- Chapter 13 of "Dive into Systems" - Threading and synchronization
- POSIX Threads Programming guide
- `man pthread_mutex_init`, `man pthread_create`
- "The Little Book of Semaphores" by Allen Downey
