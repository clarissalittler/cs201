// PEDAGOGICAL PURPOSE:
// This program demonstrates how to fix race conditions using MUTEX (mutual exclusion) locks.
// Key learning objectives:
// 1. Understanding mutexes - the primary synchronization primitive for threads
// 2. How to protect critical sections with mutex lock/unlock
// 3. The mutex lifecycle: init -> lock/unlock -> destroy
// 4. Why mutexes solve the race condition from threadRace1.c
// 5. Understanding mutual exclusion - only one thread at a time
// 6. The performance trade-off: correctness vs speed
// 7. Proper mutex usage patterns

#include <stdio.h>
#include <stdlib.h>
#include <time.h>        // For time() - seeding random
#include <pthread.h>     // POSIX threads and mutexes
#include <unistd.h>      // For sleep()

/*
 * GLOBAL COUNTER - SHARED BY ALL THREADS
 *
 * This is the same as in threadRace1.c - a shared variable that
 * multiple threads will try to modify.
 */
int ourCounter = 0;

/*
 * MUTEX - THE LOCK THAT PROTECTS THE COUNTER
 *
 * TEACHING POINT: A mutex (MUTual EXclusion) is a lock.
 * Only one thread can hold the lock at a time.
 *
 * HOW IT WORKS:
 * - Before accessing shared data, thread locks the mutex
 * - If another thread holds the lock, this thread WAITS
 * - After done with shared data, thread unlocks the mutex
 * - Waiting threads can now compete for the lock
 *
 * ANALOGY: Bathroom with one key
 * - Before entering bathroom, must have the key (lock)
 * - If someone else has key, you wait
 * - When done, you return the key (unlock)
 * - Next person can take the key
 *
 * This ensures only one person in bathroom at a time!
 */
pthread_mutex_t mutex;

/*
 * Thread function - increments counter with proper synchronization
 *
 * TEACHING POINT: This is the CORRECTED version of threadRace1.c
 * Compare this to threadRace1.c to see the difference:
 * - threadRace1.c: race condition, wrong results
 * - threadmutex.c: mutex protection, correct results
 *
 * The only difference: we added lock/unlock around the critical section!
 */
void* threadCounter(void* arg){
  /*
   * LOCK THE MUTEX before accessing shared data
   *
   * TEACHING POINT: pthread_mutex_lock() does two things:
   *
   * 1. If mutex is available (unlocked):
   *    - Acquire the lock
   *    - Continue executing
   *
   * 2. If mutex is held by another thread:
   *    - BLOCK (wait) until that thread unlocks
   *    - Then acquire the lock
   *    - Continue executing
   *
   * This ensures MUTUAL EXCLUSION - only one thread can hold
   * the lock at a time.
   */
  // we put the CRITICAL SECTION within the mutex lock/unlock
  pthread_mutex_lock(&mutex);

  /*
   * CRITICAL SECTION BEGINS
   *
   * Everything between lock and unlock is the CRITICAL SECTION.
   * Only one thread can execute this code at a time.
   *
   * TEACHING POINT: This is the exact same code as threadRace1.c,
   * but now it's protected by the mutex!
   */

  /*
   * Step 1: Read the counter
   *
   * Unlike threadRace1.c, now only ONE thread at a time can read.
   * Other threads are blocked at the lock above.
   */
  int temp = ourCounter;

  /*
   * Random delay - simulates work
   *
   * TEACHING POINT: Even with this delay, the race condition is prevented!
   * The mutex keeps other threads out during this sleep.
   *
   * In threadRace1.c, this sleep caused multiple threads to read the
   * same value. Now, one thread locks, reads, sleeps, writes, then unlocks.
   * Other threads must wait their turn.
   */
  sleep(rand()%3+1);

  /*
   * Step 2: Write back the incremented value
   *
   * Because we're still holding the lock, no other thread has
   * modified ourCounter. Our 'temp' value is still valid!
   */
  ourCounter = temp+1;

  /*
   * CRITICAL SECTION ENDS
   */

  /*
   * UNLOCK THE MUTEX to allow other threads to access shared data
   *
   * TEACHING POINT: pthread_mutex_unlock() releases the lock.
   * Any threads waiting at pthread_mutex_lock() can now compete
   * to acquire the lock.
   *
   * IMPORTANT: Always unlock what you lock!
   * - Every lock must have a corresponding unlock
   * - Forgetting to unlock = deadlock (threads stuck waiting forever)
   */
  pthread_mutex_unlock(&mutex);

  return NULL;
}

int main(){
  /*
   * Seed random number generator
   */
  srand(time(NULL));

  /*
   * Array to hold 10 thread identifiers
   */
  pthread_t threads[10];

  /*
   * Initialize the mutex
   *
   * TEACHING POINT: Must initialize mutex before use!
   *
   * Arguments:
   * 1. &mutex: pointer to the mutex to initialize
   * 2. NULL: mutex attributes (NULL = use defaults)
   *
   * Attributes could be used to configure:
   * - Process-shared (can be used across processes)
   * - Recursive (same thread can lock multiple times)
   * - Priority inheritance (for real-time systems)
   * - etc.
   *
   * For most uses, NULL (defaults) is fine.
   */
  // the second argument is for configuring it
  pthread_mutex_init(&mutex,NULL);

  /*
   * Create 10 threads
   *
   * Each thread will:
   * 1. Lock the mutex
   * 2. Increment ourCounter
   * 3. Unlock the mutex
   *
   * Because of the mutex, increments happen one at a time!
   */
  for(int i=0; i < 10; i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  /*
   * Wait for all threads to finish
   */
  for(int i=0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  /*
   * Clean up the mutex
   *
   * TEACHING POINT: After we're done with the mutex, we should destroy it.
   * This frees any resources associated with the mutex.
   *
   * MUTEX LIFECYCLE:
   * 1. pthread_mutex_init() - create and initialize
   * 2. pthread_mutex_lock/unlock() - use it
   * 3. pthread_mutex_destroy() - clean up
   */
  pthread_mutex_destroy(&mutex);

  /*
   * Print the final counter value
   *
   * TEACHING POINT: What should the counter be?
   * - 10 threads each increment by 1
   * - Should be 10
   *
   * What will it actually be?
   * - ALWAYS 10! (unlike threadRace1.c where it varied)
   *
   * WHY IS IT CORRECT NOW?
   * The mutex ensures that increments happen one at a time:
   * 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10
   * No lost updates!
   */
  printf("The value of ourCounter is: %d\n",ourCounter);

  return 0;
}

// EXECUTION TRACE (simplified, 3 threads):
//
// Time  Thread    Mutex State   temp  ourCounter  Action
// ----  ------    -----------   ----  ----------  ------
// T0    All       unlocked      -     0           Threads start
// T1    Thread0   locked(T0)    -     0           Thread0 locks mutex
//       Thread1   waiting...    -     0           Thread1 tries to lock, BLOCKS
//       Thread2   waiting...    -     0           Thread2 tries to lock, BLOCKS
// T2    Thread0   locked(T0)    0     0           Thread0 reads counter
// T3    Thread0   locked(T0)    0     0           Thread0 sleeps (still holding lock)
//       Thread1   waiting...    -     0           Thread1 still waiting
//       Thread2   waiting...    -     0           Thread2 still waiting
// T4    Thread0   locked(T0)    0     0->1        Thread0 wakes, writes 1
// T5    Thread0   unlocked      0     1           Thread0 unlocks
//       Thread1   locked(T1)    -     1           Thread1 acquires lock
//       Thread2   waiting...    -     1           Thread2 still waiting
// T6    Thread1   locked(T1)    1     1           Thread1 reads counter
// T7    Thread1   locked(T1)    1     1           Thread1 sleeps
//       Thread2   waiting...    -     1           Thread2 still waiting
// T8    Thread1   locked(T1)    1     1->2        Thread1 writes 2
// T9    Thread1   unlocked      1     2           Thread1 unlocks
//       Thread2   locked(T2)    -     2           Thread2 acquires lock
// T10   Thread2   locked(T2)    2     2           Thread2 reads counter
// T11   Thread2   locked(T2)    2     2           Thread2 sleeps
// T12   Thread2   locked(T2)    2     2->3        Thread2 writes 3
// T13   Thread2   unlocked      2     3           Thread2 unlocks
// T14   All       unlocked      -     3           Final value: 3 (correct!)

// EXPECTED OUTPUT (always the same):
// The value of ourCounter is: 10

// COMPARE TO threadRace1.c:
// threadRace1.c output varies: 1, 2, 5, 7, etc. (almost never 10)
// threadmutex.c output: always 10 (correct!)

// TRY IT:
// Compile: gcc threadmutex.c -o threadmutex -lpthread
// Run: ./threadmutex
// Run it multiple times - the output is ALWAYS 10!
//
// EXPERIMENT:
// 1. Comment out pthread_mutex_lock/unlock - see race condition return
// 2. Increase number of threads (100, 1000) - still correct!
// 3. Lock/unlock in wrong order - see what happens (deadlock!)
// 4. Forget to unlock - see what happens (deadlock!)
//
// PERFORMANCE CONSIDERATIONS:
//
// PROS OF MUTEX:
// - Guarantees correctness
// - Simple to use
// - Well-understood
//
// CONS OF MUTEX:
// - Serializes execution (threads wait for each other)
// - Can be slower than lock-free alternatives
// - Risk of deadlock if used incorrectly
//
// DESIGN GUIDELINES:
// 1. Keep critical sections SMALL
//    - Only lock what you must
//    - Don't lock during I/O if possible
//
// 2. Lock order matters
//    - If using multiple mutexes, always lock in same order
//    - Prevents deadlock
//
// 3. Always unlock
//    - Every lock must have corresponding unlock
//    - Consider using mutex guard patterns in C++
//
// 4. Don't lock recursively (same thread locking twice)
//    - Usually indicates design problem
//    - Use recursive mutex only if really needed
//
// KEY TAKEAWAYS:
// - Mutexes protect critical sections from concurrent access
// - Only one thread can hold a mutex at a time (mutual exclusion)
// - Lock before accessing shared data, unlock when done
// - Must initialize mutex before use, destroy when done
// - Mutexes trade some performance for correctness
// - The pattern: init -> lock -> critical section -> unlock -> destroy
