// PEDAGOGICAL PURPOSE:
// This program demonstrates WHY mutexes are necessary in multi-threaded programs.
// It shows the classic problem of coordinating access to shared state.
// Key learning objectives:
// 1. Understanding the problem of shared mutable state
// 2. Learning the critical section concept
// 3. Using pthread_mutex_t to protect shared data
// 4. Understanding the race condition that would occur without mutexes
// 5. Proper mutex usage pattern: init -> lock -> modify -> unlock -> destroy
// 6. Why threads must coordinate access even for simple operations
// 7. Seeing how mutexes ensure correct program behavior

#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

// GLOBAL MESSAGE ARRAY:
// Shared read-only data (safe to access without mutex)
// Each thread will print one of these messages
char* msgs[] = {"Hi there ","how are you? ","I hope this message came through"};

// SHARED COUNTER:
// This is the PROBLEM!
// Multiple threads will read and modify this variable
// WITHOUT a mutex, this would create a RACE CONDITION
int counter = 0;

// MUTEX FOR SYNCHRONIZATION:
// pthread_mutex_t is a mutual exclusion lock
// Ensures only ONE thread can access counter at a time
// This prevents race conditions and ensures correct behavior
pthread_mutex_t mut;

// THREAD WORKER FUNCTION:
// All three threads execute this same function
void* threadWorker(void* arg){

  // RANDOM DELAY:
  // Sleep 1-3 seconds (rand()%3 gives 0-2, add 1 for 1-3)
  // This makes thread execution order unpredictable
  // Simulates real-world variable execution times
  sleep(rand()%3+1);

  // LOCAL VARIABLE:
  // Each thread has its own 'temp' variable
  // This is on the thread's stack (thread-local)
  int temp;

  // CRITICAL SECTION:
  // Code between lock and unlock is the "critical section"
  // Only ONE thread can be in this section at a time
  // Other threads must WAIT at the lock until mutex is released

  // ACQUIRE MUTEX:
  // pthread_mutex_lock() acquires the mutex
  // If mutex is available: acquire it and proceed immediately
  // If another thread holds it: BLOCK (wait) until it's released
  pthread_mutex_lock(&mut);

  // ===== CRITICAL SECTION START =====
  // Only one thread at a time can execute this code

  // READ SHARED DATA:
  // Copy current counter value to thread-local variable
  // WHY? To demonstrate the read-modify-write pattern
  temp = counter;

  // MODIFY SHARED DATA:
  // Increment the shared counter
  // This determines which message this thread will print
  counter++;

  // ===== CRITICAL SECTION END =====

  // RELEASE MUTEX:
  // pthread_mutex_unlock() releases the mutex
  // Allows other waiting threads to proceed
  // Once released, we no longer have exclusive access
  pthread_mutex_unlock(&mut);

  // PRINT MESSAGE:
  // Uses 'temp' to index into msgs array
  // temp is 0, 1, or 2 depending on when thread acquired the lock
  //
  // WHY print outside critical section?
  // printf() is slow (I/O operation)
  // We want to hold locks as briefly as possible
  // Only protect the MINIMUM necessary code
  printf(msgs[temp]);

  return NULL;
}

int main(){

  // SEED RANDOM NUMBER GENERATOR:
  srand(time(NULL));

  // THREAD VARIABLES:
  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;

  // INITIALIZE MUTEX:
  // Must initialize before using!
  // Parameters:
  //   &mut - pointer to mutex to initialize
  //   NULL - use default attributes (usually fine)
  pthread_mutex_init(&mut,NULL);

  // CREATE THREADS:
  // All three threads start running immediately
  // All will try to access the shared counter
  // Mutex ensures they coordinate properly
  pthread_create(&thread1,NULL,threadWorker,NULL);
  pthread_create(&thread2,NULL,threadWorker,NULL);
  pthread_create(&thread3,NULL,threadWorker,NULL);

  // WAIT FOR COMPLETION:
  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  pthread_join(thread3,NULL);

  // FINAL OUTPUT:
  printf("\n\n");

  return 0;
  // NOTE: Missing mutex cleanup!
  // Should call: pthread_mutex_destroy(&mut);
  // Not critical here since program exits immediately
  // But good practice to clean up resources
}

// WHY DO WE NEED THE MUTEX?
//
// WITHOUT MUTEX - Race condition:
//
// Scenario: thread1 and thread2 run simultaneously
//
// Time  Thread1           Thread2           counter
// ----  -------           -------           -------
// T0                                        0
// T1    temp = counter                      0
//       (temp = 0)
// T2                      temp = counter    0
//                         (temp = 0)
// T3    counter++                           1
// T4                      counter++         2
// T5    print msgs[0]                       2
// T6                      print msgs[0]     2
//
// PROBLEM: Both threads print msgs[0] ("Hi there")!
// One message is skipped, another is duplicated
// Output might be: "Hi there Hi there I hope this message came through"
// This is a RACE CONDITION - result depends on timing

// WITH MUTEX - Correct behavior:
//
// Time  Thread1           Thread2           counter   Mutex
// ----  -------           -------           -------   -----
// T0                                        0         unlocked
// T1    lock(&mut)                          0         locked by T1
// T2    temp = counter                      0         locked by T1
//       (temp = 0)
// T3    counter++                           1         locked by T1
// T4    unlock(&mut)                        1         unlocked
// T5                      lock(&mut)        1         locked by T2
// T6                      temp = counter    1         locked by T2
//                         (temp = 1)
// T7                      counter++         2         locked by T2
// T8                      unlock(&mut)      2         unlocked
// T9    print msgs[0]                       2         unlocked
// T10                     print msgs[1]     2         unlocked
//
// RESULT: thread1 prints msgs[0], thread2 prints msgs[1]
// Each thread gets a unique index
// All three messages print exactly once
// Correct output: "Hi there how are you? I hope this message came through"
// (order may vary, but all three appear exactly once)

// CRITICAL SECTION PRINCIPLES:
//
// 1. KEEP IT SHORT:
//    - Only protect code that MUST be atomic
//    - Don't include slow operations (I/O, sleep, etc.)
//    - Here: only counter read/modify inside lock
//    - printf() outside lock (doesn't need protection)
//
// 2. PROTECT ALL ACCESSES:
//    - EVERY access to shared data must use the SAME mutex
//    - If one thread accesses counter without locking, race condition!
//    - Consistency is critical
//
// 3. ACQUIRE-MODIFY-RELEASE pattern:
//    - Lock mutex
//    - Access/modify shared data
//    - Unlock mutex
//    - This is the standard pattern

// WHAT MAKES counter++ NON-ATOMIC?
//
// In C, "counter++" looks like one operation
// But in assembly, it's THREE operations:
//
// 1. LOAD: Read counter from memory to register
// 2. ADD:  Increment register
// 3. STORE: Write register back to memory
//
// Example assembly (x86):
//   mov eax, [counter]    ; Load counter into eax
//   add eax, 1            ; Increment eax
//   mov [counter], eax    ; Store eax back to counter
//
// A context switch can happen BETWEEN these instructions!
// This is why we need mutexes even for "simple" operations

// EXECUTION TRACE (one possible ordering):
//
// Time  Thread    Mutex State        counter   Action
// ----  ------    -----------        -------   ------
// T0    main      unlocked           0         init mutex, create threads
// T1    thread2   trying to lock     0         thread2 calls lock()
// T2    thread2   locked by T2       0         thread2 acquired mutex
// T3    thread1   trying to lock     0         thread1 calls lock(), BLOCKS
// T4    thread3   trying to lock     0         thread3 calls lock(), BLOCKS
// T5    thread2   locked by T2       0         temp = 0
// T6    thread2   locked by T2       1         counter++
// T7    thread2   unlocked           1         thread2 calls unlock()
// T8    thread1   locked by T1       1         thread1 acquired mutex
// T9    thread1   locked by T1       1         temp = 1
// T10   thread1   locked by T1       2         counter++
// T11   thread1   unlocked           2         thread1 calls unlock()
// T12   thread3   locked by T3       2         thread3 acquired mutex
// T13   thread3   locked by T3       2         temp = 2
// T14   thread3   locked by T3       3         counter++
// T15   thread3   unlocked           3         thread3 calls unlock()
// T16   thread2   -                  3         prints "how are you? "
// T17   thread1   -                  3         prints "I hope this message..."
// T18   thread3   -                  3         prints "Hi there "
// T19   main      -                  3         prints "\n\n", exits

// EXPECTED OUTPUT (order varies):
// The three message parts in some order:
//   "Hi there "
//   "how are you? "
//   "I hope this message came through"
//
// Possible outputs:
// "how are you? I hope this message came throughHi there "
// "Hi there how are you? I hope this message came through"
// "I hope this message came throughHi there how are you? "
//
// All three parts appear exactly once (thanks to mutex!)
// Order is non-deterministic (random delays, OS scheduling)

// WHY temp VARIABLE?
//
// Pattern demonstrated here:
// 1. Lock mutex
// 2. Read shared data to local variable
// 3. Modify shared data
// 4. Unlock mutex
// 5. Use local variable outside critical section
//
// Benefits:
// - Minimizes time holding lock
// - Local variable is thread-safe (on thread's stack)
// - Can do slow operations (printf) without holding lock

// MUTEX PROPERTIES:
//
// 1. MUTUAL EXCLUSION: Only one thread can hold it at a time
// 2. BLOCKING: Threads wait if mutex is held
// 3. FIFO ordering (usually): Threads acquire in order they requested
// 4. REENTRANT (sometimes): Same thread can lock multiple times (not always)
// 5. OWNERSHIP: Thread that locks must unlock (good practice)

// TRY IT:
// Compile: gcc threadreview2.c -o threadreview2 -pthread
// Run: ./threadreview2
// Observe:
//   - All three messages appear exactly once
//   - Order varies between runs
//   - No duplicates, no missing messages
//
// EXPERIMENT:
// 1. Comment out pthread_mutex_lock() and pthread_mutex_unlock()
// 2. Recompile and run multiple times
// 3. You may see:
//    - Duplicate messages
//    - Missing messages
//    - Corrupted output
// 4. This demonstrates why mutexes are necessary!
//
// ANOTHER EXPERIMENT:
// Move printf(msgs[temp]) INSIDE the critical section (before unlock)
// Recompile and run
// Notice:
//   - Still works correctly
//   - But output is always in same order (deterministic)
//   - WHY? Each thread holds lock while printing
//   - Threads execute critical section atomically
//   - Less concurrency (threads can't print simultaneously)
//   - This shows tradeoff: correctness vs performance
