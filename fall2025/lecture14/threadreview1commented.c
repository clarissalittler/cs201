// PEDAGOGICAL PURPOSE:
// This program provides a gentle introduction to POSIX threads (pthreads).
// It demonstrates the fundamental concepts of thread creation and management.
// Key learning objectives:
// 1. Understanding the pthread_create() function and its parameters
// 2. Learning the required signature for thread worker functions
// 3. Understanding pthread_join() and why it's necessary
// 4. Observing non-deterministic thread execution order
// 5. Passing arguments to threads using void pointers
// 6. Reusing thread variables for sequential execution
// 7. Why printf() is generally thread-safe (unlike signal handlers)

#include "pthread.h"   // POSIX threads library
#include "unistd.h"    // For usleep()
#include "stdlib.h"    // For rand(), srand()
#include "stdio.h"     // For printf()

// INTRODUCTORY COMMENTS FROM ORIGINAL CODE:
// "let's make a simple program that spawns threads"
// "that each print a message and are done"
//
// This is the SIMPLEST possible threading example
// No shared state, no mutexes needed
// Just create threads, let them do work, wait for completion

// THREAD WORKER FUNCTION:
// This is the function that each thread will execute
//
// REQUIRED SIGNATURE:
// ALL thread worker functions must have this exact signature:
//   void* functionName(void* arg)
//
// WHY this signature?
// void* is a "generic pointer" - can point to any type
// This allows pthread_create() to work with ANY function
// This is C's way of achieving POLYMORPHISM (one interface, many types)
//
// "this looks dumb, but it's the only way to get polymorphism"
// The void* approach is inelegant but necessary in C
// In C++, you'd use templates; in Java, generics; in C, void*

void* threadWorker(void* arg){

  // CAST THE ARGUMENT:
  // arg comes in as void* (generic pointer)
  // Must cast to actual type we expect (char* in this case)
  // "gotta cast 'em all" - Pokemon reference, meaning you must cast
  char* msg = (char*)arg;

  // RANDOM DELAY:
  // usleep() = "microsecond sleep" (μ-sleep)
  // rand()%15000 generates random number 0-14999 microseconds
  // This is 0-14.999 milliseconds
  //
  // WHY add random delay?
  // Makes thread execution order non-deterministic
  // Demonstrates that we can't predict which thread finishes first
  // In real programs, threads take varying amounts of time
  usleep(rand()%15000); // usleep mu-sleep

  // PRINT THE MESSAGE:
  // "unlike with signals when we HAD to use the write syscall"
  // "printf and scanf are generally thread safe"
  //
  // WHY is printf() safe here but not in signal handlers?
  //
  // Thread safety: printf() uses locks internally
  //   - Multiple threads can call printf() simultaneously
  //   - Internal mutex prevents corruption
  //   - Safe to use in thread workers
  //
  // NOT async-signal-safe: But can deadlock in signal handlers
  //   - If signal interrupts printf(), handler's printf() deadlocks
  //   - That's why we use write() in signal handlers
  //   - But no problem in regular thread code
  printf(msg);

  // RETURN VALUE:
  // Can return any void* value (pointer to data, status code, etc.)
  // Here we return NULL (nothing interesting to return)
  //
  // "you can sorta not do this, relying on an implicit return 0;"
  // "(because 0 is also the null pointer)"
  // Technically, C allows implicit return in non-void functions
  // But explicit return NULL is clearer and better style
  return NULL;
}

// NOTE FROM ORIGINAL:
// "REMINDER go show how to re-use the thread datastructure later"
// This refers to the code at line 52-53 where thread1 is reused
// Once a thread finishes and is joined, its pthread_t can be reused

int main(){

  // SEED RANDOM NUMBER GENERATOR:
  // srand() seeds the random number generator
  // time(NULL) returns current time in seconds
  // Different seed each run = different random delays
  srand(time(NULL));

  // THREAD VARIABLES:
  // pthread_t is the thread identifier type
  // Similar to how int is a number, pthread_t identifies a thread
  // These are like "handles" or "references" to threads
  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;

  // MESSAGE STRINGS:
  // Each thread will print a different message
  // These are passed as arguments to threadWorker()
  char msg1[] = "Hi there\n";
  char msg2[] = " how are you?\n";
  char msg3[] = " I hope this message came through\n";

  // CREATE THREADS:
  // pthread_create() spawns a new thread
  //
  // PARAMETERS explained:
  // 1. pthread_t* - pointer to thread variable (output parameter)
  //    Function fills this in with new thread's ID
  //
  // 2. pthread_attr_t* - thread attributes (advanced configuration)
  //    NULL means "use default attributes"
  //    Advanced uses: set stack size, scheduling priority, etc.
  //    Usually NULL is fine
  //
  // 3. void* (*)(void*) - function pointer to worker function
  //    This is the function the thread will execute
  //    Must have signature: void* functionName(void* arg)
  //
  // 4. void* - argument passed to worker function
  //    Can pass pointer to any type
  //    Worker function receives this as its arg parameter

  // CREATE THREE THREADS:
  // All three threads start executing immediately
  // They run CONCURRENTLY (at the same time, or interleaved)
  pthread_create(&thread1,NULL,threadWorker,msg1);
  pthread_create(&thread2,NULL,threadWorker,msg2);
  pthread_create(&thread3,NULL,threadWorker,msg3);

  // WAIT FOR THREADS:
  // pthread_join() blocks until specified thread finishes
  // Parameters:
  //   1. pthread_t - which thread to wait for
  //   2. void** - pointer to receive thread's return value
  //      NULL means we don't care about return value
  //
  // WHY is join necessary?
  // If main() exits before threads finish:
  //   - Entire process terminates
  //   - All threads are killed
  //   - Their work is lost
  //
  // join() ensures:
  //   - main() waits for threads to complete
  //   - All work gets done
  //   - Clean shutdown

  // JOIN ALL THREE THREADS:
  // These calls happen SEQUENTIALLY
  // But threads might finish in any order
  //
  // Example timeline:
  // T0: All three threads created, all start running
  // T1: thread3 finishes first (random delay was shortest)
  // T2: thread1 finishes
  // T3: thread2 finishes last
  // T4: pthread_join(thread1) returns (thread1 already finished)
  // T5: pthread_join(thread2) returns (thread2 already finished)
  // T6: pthread_join(thread3) returns (thread3 already finished)
  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  pthread_join(thread3,NULL);

  // REUSING THREAD VARIABLES:
  // "REMINDER go show how to re-use the thread datastructure later"
  //
  // Once thread1 is joined, we can reuse the pthread_t variable
  // Create a NEW thread, using same variable
  // This thread runs msg1 again
  pthread_create(&thread1,NULL,threadWorker,msg1);

  // Wait for this new thread to finish
  pthread_join(thread1,NULL);

  // FINAL OUTPUT:
  printf("\n\n");

  return 0;
}

// EXECUTION TIMELINE (one possible ordering):
//
// Time  Thread    Action
// ----  ------    ------
// T0    main      Creates thread1, thread2, thread3
// T1    thread2   Starts, sleeps 5000 μs
// T2    thread1   Starts, sleeps 12000 μs
// T3    thread3   Starts, sleeps 3000 μs
// T4    thread3   Wakes up, prints " I hope this message came through\n"
// T5    thread2   Wakes up, prints " how are you?\n"
// T6    thread1   Wakes up, prints "Hi there\n"
// T7    main      pthread_join(thread1) returns
// T8    main      pthread_join(thread2) returns
// T9    main      pthread_join(thread3) returns
// T10   main      Creates new thread1
// T11   thread1   Starts, sleeps 8000 μs
// T12   thread1   Wakes up, prints "Hi there\n"
// T13   main      pthread_join(thread1) returns
// T14   main      Prints "\n\n"
// T15   main      Exits

// POSSIBLE OUTPUT:
// (Order varies due to random delays!)
//
// " I hope this message came through\n"
// " how are you?\n"
// "Hi there\n"
// "Hi there\n"
// "\n\n"
//
// Or maybe:
// "Hi there\n"
// " I hope this message came through\n"
// " how are you?\n"
// "Hi there\n"
// "\n\n"

// KEY CONCEPTS:
//
// 1. CONCURRENCY: Multiple threads run at same time
//    - Different from sequential execution
//    - Threads may interleave or run truly parallel (multi-core)
//
// 2. NON-DETERMINISM: Output order is unpredictable
//    - Random delays cause different orderings
//    - Real programs have variable execution times
//    - Can't assume thread execution order
//
// 3. THREAD LIFECYCLE:
//    - Create (pthread_create)
//    - Execute (runs worker function)
//    - Return (worker function returns)
//    - Join (pthread_join waits for completion)
//
// 4. VOID POINTERS for polymorphism:
//    - void* can point to anything
//    - Allows one pthread_create for all types
//    - Must cast back to correct type
//
// 5. THREAD SAFETY of printf():
//    - Safe in threads (uses internal locks)
//    - NOT safe in signal handlers (can deadlock)
//    - Different from async-signal-safety

// WHY NO MUTEX IN THIS PROGRAM?
//
// No shared state!
// Each thread:
//   - Has its own message
//   - Doesn't modify shared variables
//   - Just prints and exits
//
// Mutexes are only needed when:
//   - Multiple threads access same data
//   - At least one thread modifies data
//   - Without mutex: race conditions
//
// This program is safe without mutexes

// TRY IT:
// Compile: gcc threadreview1.c -o threadreview1 -pthread
// Run: ./threadreview1
// Run it multiple times
// Observe:
//   - Messages appear in different orders each run
//   - All four messages always appear
//   - No corruption or garbled output
//   - "Hi there" appears twice (once from first batch, once from reused thread)
//
// EXPERIMENT:
// 1. Change usleep to sleep(1) - makes delays visible
// 2. Remove random delay - see if order becomes deterministic (it won't, usually)
// 3. Create 100 threads instead of 3 - observe scheduling behavior
// 4. Print thread IDs with pthread_self() to see they're different
