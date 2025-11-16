// PEDAGOGICAL PURPOSE:
// This program demonstrates a RACE CONDITION - one of the most important bugs in concurrent programming.
// Key learning objectives:
// 1. Understanding race conditions and why they occur
// 2. How shared memory between threads can cause bugs
// 3. The read-modify-write problem
// 4. Why "ourCounter++" is NOT atomic (it's actually 3 operations)
// 5. Non-deterministic bugs - the result changes each run
// 6. Introduction to the need for synchronization (solved in threadmutex.c)
// 7. Critical sections - code that must execute atomically

#include <pthread.h>     // POSIX threads
#include <unistd.h>      // For sleep()
#include <fcntl.h>       // File control (not used here, but included in original)
#include <stdlib.h>      // For rand(), srand()
#include <stdio.h>       // For printf()
#include <time.h>        // For time() - seeding random

/*
 * GLOBAL VARIABLE - SHARED BY ALL THREADS
 *
 * TEACHING POINT: This variable is in the SHARED memory space.
 * ALL threads can access and modify it.
 *
 * This is different from fork():
 * - fork: each process gets its own copy
 * - threads: all threads share the same variable
 *
 * Sharing makes communication easy, but creates synchronization problems!
 */
int ourCounter = 0;

/*
 * Thread function - increments the shared counter
 *
 * TEACHING POINT: This function demonstrates a RACE CONDITION.
 * The way it's written creates a bug when multiple threads execute it.
 *
 * THE BUG:
 * This function appears to increment the counter by 1, but it's broken!
 * When multiple threads run this simultaneously, the final counter value
 * will be WRONG (less than expected).
 *
 * WHY?
 * The increment is split across a time delay, allowing threads to
 * interfere with each other.
 */
void* threadCounter(void* arg){
  /*
   * Step 1: READ the current value of ourCounter
   *
   * TEACHING POINT: We store the current value in a local variable.
   * Each thread has its own 'temp' variable (local variables are
   * NOT shared between threads - they're on each thread's stack).
   *
   * Example scenario with 2 threads:
   * - Thread A reads ourCounter (0) into its temp
   * - Thread B reads ourCounter (0) into its temp
   * Both read the same value!
   */
  // retrieve the value here
  int temp = ourCounter;

  /*
   * Random delay - simulates other work
   *
   * TEACHING POINT: This sleep makes the race condition MORE LIKELY
   * to occur. In real code, this delay might be:
   * - Computation
   * - I/O operations
   * - Just unlucky thread scheduling
   *
   * During this sleep, other threads can execute!
   * Multiple threads might read the same value before any write it back.
   *
   * Example continued:
   * - Thread A sleeps (temp=0)
   * - Thread B sleeps (temp=0)
   */
  sleep(rand()%3+1);

  /*
   * Step 2: WRITE the incremented value back
   *
   * TEACHING POINT: This is where the race condition manifests!
   * We write back (temp + 1), but temp might be STALE (out of date).
   *
   * Example continued:
   * - Thread A wakes up, writes ourCounter = 0+1 = 1
   * - Thread B wakes up, writes ourCounter = 0+1 = 1 (OVERWRITES!)
   *
   * EXPECTED: If 2 threads each increment, counter should be 2
   * ACTUAL: Counter is 1 (one increment was lost!)
   *
   * This is called a "lost update" - one thread's work is lost.
   */
  // store the incremented value down here
  // after other work was done
  ourCounter = temp +1;

  /*
   * THE CRITICAL SECTION:
   * The code from "int temp = ourCounter" to "ourCounter = temp+1"
   * is a CRITICAL SECTION - it should execute atomically (without
   * interruption), but it doesn't!
   *
   * WHY ISN'T IT ATOMIC?
   * - temp = ourCounter: LOAD from memory
   * - temp + 1: ADD operation
   * - ourCounter = temp+1: STORE to memory
   *
   * Three separate operations, with sleep in the middle!
   * Other threads can interleave, causing the race condition.
   */

  return NULL;
}

int main(){
  /*
   * Seed random number generator
   *
   * TEACHING POINT: Using time(0) as seed means different random
   * numbers each run, so the exact behavior varies.
   */
  srand(time(0));

  /*
   * Create 10 threads
   *
   * TEACHING POINT: We'll create 10 threads, each of which will
   * try to increment ourCounter once.
   *
   * EXPECTED: After all threads finish, ourCounter should be 10
   * ACTUAL: It will probably be less than 10!
   */
  pthread_t threads[10];

  /*
   * Launch all 10 threads
   *
   * All threads execute threadCounter() simultaneously (or nearly so).
   * They all access the same shared variable ourCounter.
   */
  for(int i = 0; i < 10; i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  /*
   * Wait for all threads to complete
   *
   * TEACHING POINT: We must join all threads before checking
   * the counter value, otherwise we'd read it while threads
   * are still modifying it!
   */
  for(int i = 0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  /*
   * Print the final counter value
   *
   * TEACHING POINT: What should the counter be?
   * - 10 threads each increment by 1
   * - Should be 10
   *
   * What will it actually be?
   * - Probably less than 10 (maybe 1, 2, 5, 7, ...)
   * - Different each time you run!
   * - This is a NON-DETERMINISTIC BUG
   *
   * WHY LESS THAN 10?
   * Lost updates! Multiple threads read the same value, all increment
   * it, but only the last write survives.
   */
  printf("What's the value of the counter? %d\n",ourCounter);

  return 0;
}

// EXECUTION TRACE (example with 3 threads):
//
// Time  Thread    temp  ourCounter  Action
// ----  ------    ----  ----------  ------
// T0    All       -     0           Threads start
// T1    Thread0   0     0           Reads ourCounter (0)
// T2    Thread1   0     0           Reads ourCounter (0)
// T3    Thread2   0     0           Reads ourCounter (0)
// T4    All       -     0           All sleeping...
// T5    Thread0   0     0->1        Wakes, writes 0+1=1
// T6    Thread1   0     1->1        Wakes, writes 0+1=1 (LOST UPDATE!)
// T7    Thread2   0     1->1        Wakes, writes 0+1=1 (LOST UPDATE!)
// T8    -         -     1           Final value: 1 (should be 3!)

// EXPECTED OUTPUT (will vary each run):
// What's the value of the counter? 3
// (or 2, or 5, or rarely 10)
//
// You'll almost never see 10, which is the correct answer!

// TRY IT:
// Compile: gcc threadRace1.c -o threadRace1 -lpthread
// Run: ./threadRace1
// Run it multiple times - notice the counter value changes!
//
// Typical results:
// Run 1: What's the value of the counter? 2
// Run 2: What's the value of the counter? 1
// Run 3: What's the value of the counter? 4
// Run 4: What's the value of the counter? 1
//
// EXPERIMENT:
// 1. Remove the sleep() - race becomes less likely but still possible
// 2. Increase the number of threads - makes the bug more obvious
// 3. Reduce sleep time - see how it affects the bug
//
// WHY IS THIS DANGEROUS?
// - Non-deterministic: bug appears sometimes, not always
// - Hard to debug: works in testing, fails in production
// - Silent corruption: wrong results, no error message
// - Heisenbug: adding debug prints might make it disappear!
//
// THE SOLUTION:
// We need SYNCHRONIZATION to protect the critical section.
// See threadmutex.c to learn how to fix this with a mutex!
//
// KEY TAKEAWAY:
// Shared memory is convenient but dangerous. Any time multiple
// threads access shared data, and at least one modifies it,
// you have a potential race condition that needs synchronization.
