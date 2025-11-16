// PEDAGOGICAL PURPOSE:
// This program demonstrates a classic RACE CONDITION in multi-threaded programming.
// Key learning objectives:
// 1. Understanding what a race condition is and why it occurs
// 2. Seeing how concurrent access to shared data causes incorrect results
// 3. Learning that the final result is NON-DETERMINISTIC (unpredictable)
// 4. Understanding the read-modify-write problem
// 5. Recognizing that this is a BROKEN program (intentionally!)
// 6. Motivating the need for synchronization mechanisms (mutex, locks)
// 7. Understanding why counter will almost never reach 100

#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"     // For sleep() function

// SHARED GLOBAL VARIABLE:
// This is the CRITICAL piece - a variable shared among ALL threads
// Multiple threads will read and write this variable CONCURRENTLY
// This shared access without synchronization is the source of the race condition
//
// EXPECTED: If 100 threads each increment this once, it should be 100
// ACTUAL: It will be much less than 100 (often 1, 2, or 3)
int counter = 0;

// THREAD FUNCTION:
// This function will be executed by EACH thread
// All 100 threads run this same code concurrently
//
// SIGNATURE REQUIRED BY PTHREAD:
// void* incer(void* arg)
//   - Must return void* (generic pointer)
//   - Must take void* as parameter (even if unused)
//   - This is the standard signature for pthread functions
void* incer(void* arg){

  // THE RACE CONDITION HAPPENS HERE:
  // This looks like simple code, but it's actually THREE operations:
  //
  // Step 1: READ counter into temp
  // Step 2: (simulated work - sleep)
  // Step 3: WRITE temp+1 back to counter
  //
  // WHY THIS IS BROKEN:
  // Multiple threads can interleave these steps in problematic ways!
  int temp = counter;

  // SIMULATED WORK:
  // sleep(rand()%3+1) pauses this thread for 1-3 seconds
  // This simulates "work" happening between read and write
  //
  // WHY THE SLEEP?
  // The sleep EXAGGERATES the race condition to make it obvious
  // Even without sleep, race conditions occur, but they're harder to observe
  // because modern CPUs are so fast the window is microseconds
  //
  // THE PROBLEM:
  // While this thread sleeps, OTHER threads also read counter
  // Many threads might all read counter = 0
  // Then they all write back counter = 1
  // So 100 increments only increases counter by 1!
  sleep(rand()%3+1); // simulated work

  // THE WRITE:
  // We write temp+1 back to counter
  // But temp was read BEFORE the sleep
  // Other threads may have updated counter in the meantime
  // Those updates will be LOST because we overwrite with our stale value
  counter = temp+1;

  // THREAD TERMINATION:
  // Return NULL (required by pthread signature)
  return NULL;
}

int main(){
  // SEED THE RANDOM NUMBER GENERATOR:
  // srand(time(NULL)) initializes random number generation
  // This ensures different sleep durations each run
  // time(NULL) returns current time in seconds since epoch
  srand(time(NULL));

  // THREAD ARRAY:
  // pthread_t is an opaque type representing a thread
  // We create an array to hold 100 thread identifiers
  // We'll create 100 threads, all running the incer function
  pthread_t threads[100];

  // CREATE 100 THREADS:
  // Each thread will execute the incer function
  for(int i=0; i<100;i++){
    // PTHREAD_CREATE:
    // int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    //                    void *(*start_routine)(void*), void *arg)
    //
    // PARAMETERS:
    // threads+i: Pointer to where thread ID should be stored (threads[i])
    // NULL: Use default thread attributes
    // incer: The function this thread will execute
    // NULL: No argument to pass to incer (arg parameter unused)
    //
    // WHAT HAPPENS:
    // A new thread is created and immediately starts executing incer()
    // This happens CONCURRENTLY with the main thread
    // All threads are running simultaneously (on multi-core systems)
    pthread_create(threads+i,NULL,incer,NULL);
  }

  // WAIT FOR ALL THREADS TO COMPLETE:
  // pthread_join() blocks until the specified thread terminates
  // This is like wait() for processes
  for(int i=0; i<100;i++){
    // PTHREAD_JOIN:
    // int pthread_join(pthread_t thread, void **retval)
    //
    // PARAMETERS:
    // threads[i]: The thread to wait for
    // NULL: We don't care about the return value from the thread
    //
    // WHY NECESSARY?
    // Without join, main might exit before threads finish
    // This would terminate all threads prematurely
    // Join ensures we wait for all threads to complete their work
    pthread_join(threads[i],NULL);
  }

  // PRINT THE RESULT:
  // WHAT WE EXPECT: 100 (each of 100 threads increments once)
  // WHAT WE GET: Probably 1, 2, or 3 (sometimes up to ~10)
  //
  // WHY SO LOW?
  // Because of the sleep, many threads read counter while it's still 0
  // They all compute 0+1 = 1
  // They all write 1 back
  // So despite 100 increments, counter only reaches 1!
  printf("The counter is: %d\n",counter);


  return 0;
}

// EXPECTED OUTPUT (will vary each run):
// The counter is: 1
//   OR
// The counter is: 2
//   OR
// The counter is: 3
//
// RARELY will you see values above 5-10
// NEVER will you see 100

// DETAILED RACE CONDITION ANALYSIS:
//
// SCENARIO: What happens with just 3 threads
//
// Time  Thread1        Thread2        Thread3        counter value
// ----  -------        -------        -------        -------------
// T0    reads (0)                                    0
// T1                   reads (0)                     0
// T2                                  reads (0)      0
// T3    sleeping...    sleeping...    sleeping...   0
// T4    sleeping...    sleeping...    sleeping...   0
// T5    writes 1       sleeping...    sleeping...   1
// T6                   writes 1       sleeping...   1  <- Thread2's update LOST!
// T7                                  writes 1       1  <- Thread3's update LOST!
//
// Final counter = 1, but THREE increments happened!
// TWO updates were lost because threads overwrote each other

// THE READ-MODIFY-WRITE PROBLEM:
//
// Even the simple statement "counter++;" is not atomic!
// It compiles to THREE assembly instructions:
//
// 1. LOAD counter from memory into a register
// 2. INCREMENT the register value
// 3. STORE the register back to memory
//
// Thread interleaving can occur between ANY of these steps!
//
// Example with counter starting at 0:
// Thread A: LOAD (gets 0)
// Thread B: LOAD (gets 0)
// Thread A: INCREMENT (register = 1)
// Thread B: INCREMENT (register = 1)
// Thread A: STORE (counter = 1)
// Thread B: STORE (counter = 1)
// Result: counter = 1, but it should be 2!

// WHY THIS MATTERS:
//
// 1. CORRECTNESS:
//    Race conditions lead to incorrect results
//    The program's behavior is unpredictable
//
// 2. DEBUGGING NIGHTMARE:
//    Results vary from run to run
//    Hard to reproduce bugs
//    "Works on my machine" syndrome
//
// 3. REAL-WORLD IMPACT:
//    Banking: Lost transactions
//    Gaming: Incorrect scores
//    Web servers: Corrupted data
//
// 4. NEED FOR SYNCHRONIZATION:
//    This motivates mutexes, locks, semaphores, atomic operations

// THE SOLUTION (preview):
// The next examples (threadRace2.c, threadRace3.c) show how to fix this
// using MUTEXES (mutual exclusion locks)

// TRY IT:
// 1. Compile: gcc threadRace.c -o threadRace -pthread
//    Note: -pthread flag is REQUIRED for pthread programs
// 2. Run multiple times: ./threadRace
// 3. Observe that the counter is almost never 100
// 4. Notice that results vary from run to run
// 5. EXPERIMENT: Remove the sleep() - does it still show race conditions?
//    (It still happens, but might be harder to observe)
// 6. EXPERIMENT: Reduce to 10 threads - easier to reason about
// 7. EXPERIMENT: Print from inside incer to see thread interleaving
//    printf("Thread reading: %d\n", temp); (but this adds its own race conditions!)
