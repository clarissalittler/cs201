// PEDAGOGICAL PURPOSE:
// This program demonstrates the critical role of mutexes in protecting shared state.
// It shows how mutexes solve race conditions by ensuring atomic read-modify-write operations.
// Key learning objectives:
// 1. Understanding race conditions with shared variables
// 2. Learning pthread_mutex_t and mutex operations
// 3. Understanding critical sections and atomicity
// 4. Seeing why simple increment operations need mutex protection
// 5. Learning the mutex lifecycle: init, lock, unlock, destroy
// 6. Understanding the difference between protecting data vs. code
// 7. Observing the performance impact of mutex locking
// 8. Learning proper mutex initialization and cleanup

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// SHARED GLOBAL VARIABLE:
// This is the data that multiple threads will access concurrently
// WITHOUT protection, this would be a RACE CONDITION
// WITH mutex protection, we ensure only one thread modifies it at a time
//
// WHY GLOBAL?
// All threads in a process share the same memory space
// Global variables are accessible to all threads
// This makes them convenient for sharing data, but DANGEROUS without protection
int ourCounter = 0;

// MUTEX DECLARATION:
// pthread_mutex_t is an opaque type representing a mutual exclusion lock
// "Mutex" = MUTual EXclusion
//
// WHAT IS A MUTEX?
// Think of it as a lock on a bathroom door:
//   - Only one person can be inside at a time
//   - Others must wait until it's unlocked
//   - First come, first served (usually)
//
// WHY GLOBAL?
// The mutex must be accessible to all threads that need to protect the shared data
// All threads must use the SAME mutex to protect the SAME data
pthread_mutex_t mutex;

// THREAD WORKER FUNCTION:
// Each thread executes this function
// Ten threads will all run this concurrently
//
// CRITICAL SECTION:
// The code between lock and unlock is the "critical section"
// Only ONE thread can execute this code at any given time
void* threadCounter(void* arg){

  // LOCK THE MUTEX:
  // pthread_mutex_lock acquires the mutex
  //
  // WHAT HAPPENS:
  // - If mutex is unlocked: Thread acquires it and continues
  // - If mutex is locked: Thread BLOCKS (waits) until it's unlocked
  //
  // BLOCKING:
  // The thread goes to sleep until the mutex becomes available
  // The OS scheduler will wake it up when the mutex is unlocked
  //
  // WHY LOCK HERE?
  // We're about to access and modify shared data (ourCounter)
  // We must ensure no other thread interferes during our read-modify-write
  pthread_mutex_lock(&mutex);

  // READ PHASE:
  // Read the current value of the counter
  // Store it in a local variable
  //
  // WHY IS THIS DANGEROUS WITHOUT MUTEX?
  // Consider two threads without mutex:
  //   T0: Thread1 reads ourCounter (value: 0)
  //   T1: Thread2 reads ourCounter (value: 0)
  //   T2: Thread1 sleeps
  //   T3: Thread2 sleeps
  //   T4: Thread1 wakes, writes 0+1=1
  //   T5: Thread2 wakes, writes 0+1=1
  //   RESULT: Counter is 1 instead of 2! (Lost update)
  int temp = ourCounter;

  // SIMULATE SLOW OPERATION:
  // Sleep for 1-3 seconds (random)
  // This exaggerates the race condition window
  //
  // WHY SLEEP?
  // In real programs, the read-modify-write might be interrupted by:
  //   - Context switches
  //   - Page faults
  //   - Other system events
  // sleep() makes the vulnerability obvious for learning purposes
  //
  // WITH MUTEX:
  // While this thread sleeps, other threads are BLOCKED at pthread_mutex_lock
  // They cannot proceed until this thread unlocks
  // This ensures atomicity despite the sleep
  sleep(rand()%3+1);

  // WRITE PHASE:
  // Update the counter with the new value
  //
  // THE CRITICAL OPERATION:
  // This is a read-modify-write operation:
  //   1. Read temp (already done)
  //   2. Compute temp+1
  //   3. Write result to ourCounter
  //
  // WITHOUT MUTEX: This three-step operation can be interleaved with other threads
  // WITH MUTEX: This three-step operation is ATOMIC (appears instantaneous)
  ourCounter = temp+1;

  // UNLOCK THE MUTEX:
  // Release the mutex so other threads can proceed
  //
  // WHAT HAPPENS:
  // - Mutex becomes available
  // - One waiting thread (if any) will be woken up
  // - That thread acquires the mutex and enters the critical section
  //
  // FAIRNESS:
  // pthread mutexes are typically fair (first-come-first-served)
  // But this is NOT guaranteed by POSIX
  // Some implementations may allow "barging" (newer thread grabs mutex first)
  pthread_mutex_unlock(&mutex);

  // RETURN:
  // Thread completes successfully
  return NULL;
}

int main(){

  // SEED RANDOM NUMBER GENERATOR:
  // Different seed each run = different sleep patterns
  // Makes timing non-deterministic
  srand(time(NULL));

  // THREAD ARRAY:
  // We'll create 10 threads
  // Array allows us to track all thread IDs
  pthread_t threads[10];

  // INITIALIZE MUTEX:
  // pthread_mutex_init() must be called before using a mutex
  //
  // PARAMETERS:
  // 1. pthread_mutex_t* - pointer to mutex variable
  // 2. pthread_mutexattr_t* - attributes (NULL = default)
  //
  // DEFAULT ATTRIBUTES:
  // - Type: Normal (fast but no error checking)
  // - Process-shared: No (only threads in this process can use it)
  // - Protocol: None (no priority inheritance)
  //
  // ALTERNATIVE INITIALIZATION:
  // For static mutexes, can use:
  //   pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  // But dynamic initialization with pthread_mutex_init is more flexible
  pthread_mutex_init(&mutex,NULL);

  // CREATE 10 THREADS:
  // All threads will compete to increment ourCounter
  // Each thread should increment it once
  // Final value should be 10 (if mutex works correctly)
  //
  // EXPECTED BEHAVIOR WITHOUT MUTEX:
  // Final value would be LESS than 10 (race condition)
  // Some increments would be lost
  //
  // EXPECTED BEHAVIOR WITH MUTEX:
  // Final value will ALWAYS be 10
  // No increments lost
  for(int i=0; i < 10; i++){
    // CREATE THREAD:
    // Each thread executes threadCounter with no arguments
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  // WAIT FOR ALL THREADS:
  // Join each thread to ensure they all complete
  // Main thread blocks until all worker threads finish
  //
  // WHY NECESSARY?
  // If main exits before threads finish:
  //   - Entire process terminates
  //   - Threads are killed
  //   - ourCounter might not reach 10
  for(int i=0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  // CLEANUP: DESTROY MUTEX:
  // pthread_mutex_destroy() releases mutex resources
  //
  // WHEN TO DESTROY:
  // - After all threads are done using it
  // - Before program exits (good practice)
  // - After pthread_join ensures no thread is using it
  //
  // WHAT HAPPENS:
  // - Mutex becomes invalid
  // - Using it after destroy is UNDEFINED BEHAVIOR
  // - System can reclaim internal resources
  //
  // IMPORTANT:
  // - Don't destroy a locked mutex (undefined behavior)
  // - Don't destroy a mutex while threads might use it
  // - Always destroy after pthread_join completes
  pthread_mutex_destroy(&mutex);

  // PRINT RESULT:
  // Should always be 10 if mutex worked correctly
  // If less than 10, there's a bug (mutex not protecting correctly)
  printf("The value of ourCounter is: %d\n",ourCounter);

  return 0;
}

// EXPECTED OUTPUT:
// The value of ourCounter is: 10

// EXECUTION TIMELINE (simplified):
//
// Time  Thread   Action                        ourCounter  Mutex State
// ----  ------   ------                        ----------  -----------
// T0    main     Creates 10 threads            0           unlocked
// T1    thread0  pthread_mutex_lock succeeds   0           locked by thread0
// T2    thread1  pthread_mutex_lock blocks     0           locked by thread0
// T3    thread2  pthread_mutex_lock blocks     0           locked by thread0
// ...   (other threads also block)
// T10   thread0  temp = 0                      0           locked by thread0
// T11   thread0  sleep(2) begins               0           locked by thread0
//       (thread0 sleeps but HOLDS THE MUTEX)
// T12   thread1  (still blocked)               0           locked by thread0
// T13   thread0  sleep(2) ends                 0           locked by thread0
// T14   thread0  ourCounter = 1                1           locked by thread0
// T15   thread0  pthread_mutex_unlock          1           unlocked
// T16   thread1  pthread_mutex_lock succeeds   1           locked by thread1
// T17   thread1  temp = 1                      1           locked by thread1
// T18   thread1  sleep(3) begins               1           locked by thread1
// ...   (continues for all 10 threads)
// T100  thread9  ourCounter = 10               10          locked by thread9
// T101  thread9  pthread_mutex_unlock          10          unlocked
// T102  main     All joins complete            10          unlocked
// T103  main     Prints result: 10             10          destroyed

// WHAT WOULD HAPPEN WITHOUT MUTEX?
//
// Consider just 2 threads without mutex protection:
//
// Time  Thread1         Thread2         ourCounter
// ----  -------         -------         ----------
// T0    temp = 0                        0
// T1                    temp = 0        0
// T2    sleep(1)                        0
// T3                    sleep(2)        0
// T4    ourCounter = 1                  1
// T5                    ourCounter = 1  1  <- LOST UPDATE!
//
// Final value: 1 (should be 2)
//
// With 10 threads, typical result without mutex: 3-7 (should be 10)

// KEY CONCEPTS:
//
// 1. RACE CONDITION:
//    When multiple threads access shared data
//    At least one thread modifies the data
//    No synchronization mechanism
//    Result: Unpredictable, incorrect behavior
//
// 2. CRITICAL SECTION:
//    Code that accesses shared data
//    Must be protected by synchronization
//    Only one thread executes it at a time
//    Between lock and unlock
//
// 3. ATOMICITY:
//    An operation appears to happen instantaneously
//    No other thread can see intermediate states
//    Read-modify-write must be atomic
//    Mutex provides atomicity
//
// 4. MUTUAL EXCLUSION:
//    Only one thread in critical section
//    Others must wait
//    Prevents race conditions
//    Ensures data consistency
//
// 5. MUTEX SEMANTICS:
//    lock: Acquire exclusive access (block if necessary)
//    unlock: Release exclusive access (wake waiting thread)
//    Only the thread that locked can unlock (POSIX requirement)
//    Unlocking an unlocked mutex is undefined behavior

// WHY THE SLEEP?
//
// Without sleep, the race condition is VERY unlikely:
//   - temp = ourCounter; ourCounter = temp+1;
//   - These execute in nanoseconds
//   - Very small window for interleaving
//   - Race condition might happen 1 in 1000 runs
//
// With sleep, the race condition is GUARANTEED without mutex:
//   - Window is 1-3 seconds
//   - Plenty of time for other threads to interfere
//   - Makes the bug obvious and reproducible
//
// In real programs:
//   - Operations are faster than sleep
//   - But interruptions still happen (context switch, page fault, etc.)
//   - Race conditions are rare but WILL happen eventually
//   - Better to prevent with mutex than debug later

// MUTEX OVERHEAD:
//
// Mutexes have cost:
//   - Lock/unlock are system calls (can be slow)
//   - Contention causes blocking (threads wait)
//   - Sequential execution in critical section (no parallelism)
//
// Without mutex (if it were safe):
//   - All 10 threads could run in parallel
//   - On 10-core system, might finish in ~2 seconds
//
// With mutex:
//   - Threads run sequentially in critical section
//   - 10 threads × ~2 seconds = ~20 seconds total
//
// But correctness matters more than speed!
// Better to be slow and correct than fast and wrong

// DESIGN PRINCIPLE:
//
// "Protect data, not code"
//
// You don't protect the code (threadCounter function)
// You protect the DATA (ourCounter variable)
//
// The mutex guards access to specific shared data
// Any code that accesses that data must use the same mutex
//
// WRONG THINKING: "This function is protected"
// RIGHT THINKING: "This data is protected by this mutex"

// COMPILE AND RUN:
// gcc threadmutex.c -o threadmutex -pthread
// ./threadmutex
//
// EXPERIMENT:
// 1. Comment out pthread_mutex_lock and pthread_mutex_unlock
//    Run multiple times
//    Observe: Final value is usually less than 10
//
// 2. Change sleep to usleep(1) (1 microsecond)
//    Even with mutex, should be faster
//    Still correct!
//
// 3. Increase to 100 threads
//    Should still get 100 (with mutex)
//    Without mutex, would get much less
//
// 4. Try locking AFTER reading temp
//    pthread_mutex_lock(&mutex); after temp = ourCounter;
//    Observe: Back to race condition! Lock must be BEFORE read.
//
// 5. Add print statements inside critical section
//    printf("Thread entering critical section\n");
//    Observe: Only one thread prints at a time
