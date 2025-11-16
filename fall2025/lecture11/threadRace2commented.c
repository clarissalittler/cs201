// PEDAGOGICAL PURPOSE:
// This program demonstrates how to FIX the race condition from threadRace.c using a MUTEX.
// Key learning objectives:
// 1. Understanding what a mutex (mutual exclusion lock) is and how it works
// 2. Learning how to declare, initialize, and destroy a mutex
// 3. Understanding pthread_mutex_lock() and pthread_mutex_unlock()
// 4. Seeing how mutexes ensure only ONE thread accesses critical section at a time
// 5. Understanding the concept of a "critical section" in concurrent programming
// 6. Learning that proper synchronization guarantees correct results
// 7. Understanding the performance tradeoff of synchronization (slower but correct)

#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


// SHARED GLOBAL VARIABLE:
// Same as before - shared among all threads
// But now it will be PROTECTED by a mutex
int counter = 0;

// MUTEX DECLARATION:
// pthread_mutex_t is an opaque type representing a mutual exclusion lock
// This is a GLOBAL mutex that all threads will use
//
// WHAT IS A MUTEX?
// Mutex = Mutual Exclusion
// It's like a lock on a bathroom door:
//   - Only ONE person (thread) can be inside at a time
//   - Others must WAIT outside until the door is unlocked
//   - When you leave, you unlock so others can enter
pthread_mutex_t mutex;


// THREAD FUNCTION (NOW WITH MUTEX PROTECTION):
// This is the CORRECTED version of the incer function
// The critical section is now protected by a mutex
void* incer(void* arg){
  // ACQUIRE THE LOCK (LOCK THE MUTEX):
  // pthread_mutex_lock(&mutex) attempts to lock the mutex
  //
  // TWO POSSIBLE OUTCOMES:
  // 1. Mutex is unlocked: This thread acquires it and proceeds immediately
  // 2. Mutex is locked by another thread: This thread BLOCKS (waits) until it's unlocked
  //
  // GUARANTEE:
  // Only ONE thread can hold the mutex at any time
  // All other threads wait in line
  //
  // ANALOGY:
  // This is like grabbing a "talking stick" in a meeting
  // Only the person with the stick can talk
  // Everyone else waits their turn
  pthread_mutex_lock(&mutex); // only one thread will win, everyone else waits until the mutex is *un* locked

  // CRITICAL SECTION BEGINS:
  // Everything between lock() and unlock() is the CRITICAL SECTION
  // Only ONE thread can execute this code at a time
  // This is the code that accesses shared data (counter)
  //
  // THE PROTECTION:
  // While this thread is in the critical section, ALL other threads are waiting
  // Even if they want to read or write counter, they can't - they're blocked at the lock() call

  // READ counter (protected):
  int temp = counter;

  // SIMULATED WORK:
  // Even with the sleep, we're safe now!
  // While this thread sleeps, other threads are WAITING at the lock
  // They cannot read counter until this thread unlocks
  sleep(rand()%3+1); // simulated work

  // WRITE counter (protected):
  // We write back to counter
  // No other thread can interfere because we hold the lock
  counter = temp+1;

  // RELEASE THE LOCK (UNLOCK THE MUTEX):
  // pthread_mutex_unlock(&mutex) releases the mutex
  //
  // WHAT HAPPENS:
  // 1. This thread releases the lock
  // 2. ONE of the waiting threads (if any) acquires the lock
  // 3. That thread can now enter the critical section
  //
  // CRITICAL SECTION ENDS
  pthread_mutex_unlock(&mutex);

  // THREAD TERMINATION:
  return NULL;
}

int main(){
  // SEED RANDOM NUMBER GENERATOR:
  srand(time(NULL));

  // THREAD ARRAY:
  // Reduced to 5 threads instead of 100
  // This makes it easier to see the effect while still demonstrating concurrency
  // (With 100 threads and sleep, this would take a VERY long time!)
  pthread_t threads[5];

  // INITIALIZE THE MUTEX:
  // pthread_mutex_init() must be called before using the mutex
  //
  // PARAMETERS:
  // &mutex: Pointer to the mutex to initialize
  // NULL: Use default mutex attributes
  //
  // WHEN TO INITIALIZE:
  // BEFORE any threads are created
  // The mutex must be ready before threads try to use it
  //
  // WHAT THIS DOES:
  // Sets up the internal state of the mutex
  // Marks it as "unlocked" and "available"
  pthread_mutex_init(&mutex,NULL); // pass the pointer to the mutex

  // CREATE 5 THREADS:
  // Each thread will execute incer()
  // All threads will compete for the same mutex
  for(int i=0; i<5;i++){
    pthread_create(threads+i,NULL,incer,NULL);
  }

  // WAIT FOR ALL THREADS TO COMPLETE:
  // Same as before - join all threads
  for(int i=0; i<5;i++){
    pthread_join(threads[i],NULL);
  }

  // PRINT THE RESULT:
  // NOW the result should ALWAYS be 5
  // Each of 5 threads successfully increments counter exactly once
  // No lost updates!
  printf("The counter is: %d\n",counter);

  // DESTROY THE MUTEX:
  // pthread_mutex_destroy() cleans up the mutex
  //
  // WHEN TO DESTROY:
  // AFTER all threads have finished using it
  // Before the program exits
  //
  // WHAT THIS DOES:
  // Frees any resources associated with the mutex
  // Marks the mutex as invalid
  //
  // NOTE:
  // Destroying a locked mutex is undefined behavior (don't do it!)
  pthread_mutex_destroy(&mutex);

  return 0;
}

// EXPECTED OUTPUT:
// The counter is: 5
//
// EVERY TIME you run this, you should get 5
// The result is now DETERMINISTIC (predictable)

// HOW MUTEXES WORK:
//
// EXECUTION TIMELINE with mutex (5 threads):
//
// Time  Thread1        Thread2        Thread3        Thread4        Thread5        counter
// ----  -------        -------        -------        -------        -------        -------
// T0    lock (OK)      lock (WAIT)    lock (WAIT)    lock (WAIT)    lock (WAIT)    0
// T1    read (0)       [waiting]      [waiting]      [waiting]      [waiting]      0
// T2    sleep...       [waiting]      [waiting]      [waiting]      [waiting]      0
// T3    sleep...       [waiting]      [waiting]      [waiting]      [waiting]      0
// T4    write 1        [waiting]      [waiting]      [waiting]      [waiting]      1
// T5    unlock         [waiting]      [waiting]      [waiting]      [waiting]      1
// T6                   lock (OK)      [waiting]      [waiting]      [waiting]      1
// T7                   read (1)       [waiting]      [waiting]      [waiting]      1
// T8                   sleep...       [waiting]      [waiting]      [waiting]      1
// T9                   sleep...       [waiting]      [waiting]      [waiting]      1
// T10                  write 2        [waiting]      [waiting]      [waiting]      2
// T11                  unlock         [waiting]      [waiting]      [waiting]      2
// T12                                 lock (OK)      [waiting]      [waiting]      2
// ...and so on...
// Final result: 5 (correct!)

// KEY CONCEPTS:
//
// 1. CRITICAL SECTION:
//    Code between lock() and unlock()
//    Only one thread can execute this at a time
//    This is where shared data is accessed
//
// 2. MUTUAL EXCLUSION:
//    Guarantee that only ONE thread is in the critical section
//    All other threads must wait
//
// 3. ATOMICITY:
//    The entire critical section executes as one "atomic" operation
//    From the outside, it appears instantaneous and uninterruptible
//
// 4. BLOCKING:
//    Threads that can't acquire the lock are BLOCKED
//    They don't spin/busy-wait, they sleep until the lock is available
//    The OS scheduler wakes them when the lock is released

// MUTEX LIFECYCLE:
//
// 1. DECLARE: pthread_mutex_t mutex;
// 2. INITIALIZE: pthread_mutex_init(&mutex, NULL);
// 3. USE: pthread_mutex_lock(&mutex); ... pthread_mutex_unlock(&mutex);
// 4. DESTROY: pthread_mutex_destroy(&mutex);
//
// NEVER skip step 2 or 4!

// COMMON PITFALLS:
//
// 1. FORGETTING TO UNLOCK:
//    If you lock but never unlock, all other threads will deadlock
//    Always ensure every lock has a matching unlock
//
// 2. UNLOCKING TWICE:
//    Unlocking an already-unlocked mutex is undefined behavior
//
// 3. WRONG MUTEX:
//    Using different mutexes doesn't provide protection
//    All threads must use the SAME mutex for the SAME shared data
//
// 4. FORGETTING TO INITIALIZE:
//    Using an uninitialized mutex is undefined behavior
//
// 5. DEADLOCK:
//    Thread A locks mutex1, waits for mutex2
//    Thread B locks mutex2, waits for mutex1
//    Both threads wait forever!

// PERFORMANCE CONSIDERATIONS:
//
// COMPARISON: threadRace.c vs threadRace2.c
//
// threadRace.c (no mutex):
//   - FAST: Threads run truly in parallel
//   - WRONG: Race conditions lead to incorrect results
//   - UNPREDICTABLE: Results vary from run to run
//
// threadRace2.c (with mutex):
//   - SLOWER: Threads must wait for each other
//   - CORRECT: Guaranteed correct results
//   - PREDICTABLE: Always gives the same answer
//
// THE TRADEOFF:
// We sacrifice some parallelism (speed) for correctness
// But correct and slow is better than fast and wrong!

// TRY IT:
// 1. Compile: gcc threadRace2.c -o threadRace2 -pthread
// 2. Run multiple times: ./threadRace2
// 3. Observe that the counter is ALWAYS 5
// 4. EXPERIMENT: Comment out the lock() and unlock() calls
//    You'll see the race condition return!
// 5. EXPERIMENT: Comment out just the unlock()
//    The program will hang (deadlock) - only first thread completes
// 6. EXPERIMENT: Use two different mutexes (mutex1 and mutex2)
//    You'll see race conditions return because threads aren't actually synchronized
// 7. COMPARE: Run threadRace.c and threadRace2.c side by side
//    threadRace.c: wrong answer, fast
//    threadRace2.c: right answer, slower
