// PEDAGOGICAL PURPOSE:
// This program demonstrates passing arguments TO threads (not just receiving returns).
// It shows the fourth parameter of pthread_create and void* argument casting.
// Key learning objectives:
// 1. Understanding pthread_create's fourth parameter (thread argument)
// 2. Learning how to pass data to thread functions
// 3. Understanding void* casting for passing arguments
// 4. Learning the cast pattern: *(int*)arg
// 5. Understanding why we pass addresses (&sleepArg1)
// 6. Seeing practical use of thread arguments for different behavior
// 7. Understanding the complete thread pattern: data in, process, data out
// 8. Learning about stack variable lifetime and thread safety

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

// COMMENT FROM ORIGINAL CODE:
// "when you see void*, that's polymorphism in C"
//
// This applies to BOTH:
// - The argument (void* arg) - can be any pointer type
// - The return (void*) - can be any pointer type

// THREAD FUNCTION:
// This thread receives a parameter telling it how long to sleep
//
// WHAT'S NEW:
// The arg parameter is actually USED now!
// Previous examples passed NULL and ignored arg
// This example passes actual data via arg
void* func1(void* arg){

  // EXTRACT THE ARGUMENT:
  // arg is void* (generic pointer)
  // We know it's actually int* (we passed it)
  // Must cast to int* to use it
  //
  // THE CAST: *(int*)arg
  // Let's break this down step by step:
  //
  // STEP 1: (int*)arg
  //   Cast void* to int*
  //   Now we have a pointer to an integer
  //
  // STEP 2: *(int*)arg
  //   Dereference the int*
  //   Now we have the actual integer value
  //
  // COMMON MISTAKE:
  //   int sleepAmount = (int)arg;  // WRONG!
  //   This casts the POINTER to int, not the VALUE
  //
  // CORRECT:
  //   int sleepAmount = *(int*)arg;  // RIGHT!
  //   This gets the value the pointer points to
  int sleepAmount = *(int*) arg;

  // SLEEP FOR SPECIFIED DURATION:
  // Use the argument we received
  // Each thread gets different sleep duration
  // thread1 might get 3, thread2 might get 5
  sleep(sleepAmount);

  // PRINT COMPLETION MESSAGE:
  // Show that thread finished and how long it slept
  // The %d shows the sleep duration
  //
  // NON-DETERMINISTIC OUTPUT:
  // Threads will print in order of COMPLETION, not CREATION
  // Shorter sleep finishes first
  //
  // EXAMPLE:
  // If thread1 sleeps 5 seconds, thread2 sleeps 2 seconds
  // Output order:
  //   [Yawn] I slept for 2 seconds  (thread2 finishes first)
  //   [Yawn] I slept for 5 seconds  (thread1 finishes second)
  printf("[Yawn] I slept for %d seconds\n",sleepAmount);

  // RETURN NULL:
  // No data to return (we already printed)
  // Could return data if needed (like threads3.c)
  return NULL;
}

int main(){

  // SEED RANDOM NUMBER GENERATOR:
  // For generating random sleep durations
  srand(time(NULL));

  // THREAD HANDLES:
  pthread_t thread1;
  pthread_t thread2;

  // ARGUMENTS FOR THREADS:
  // These are the values we'll pass to our threads
  // Each thread gets its own argument
  //
  // WHY SEPARATE VARIABLES?
  // Each thread needs its own data
  // If we used same variable, both threads would see same value
  //
  // LIFETIME ISSUE:
  // These are stack variables in main()
  // Are they safe to pass to threads?
  // YES, because:
  //   - main() doesn't exit until after pthread_join
  //   - Threads finish before main() stack is destroyed
  //   - Safe to pass addresses of these variables
  //
  // DANGEROUS PATTERN (don't do this!):
  //   void create_thread() {
  //     int arg = 42;
  //     pthread_create(&thread, NULL, func1, &arg);
  //     return;  // DANGER! arg destroyed, thread might still use it
  //   }
  int sleepArg1 = rand()%6 + 1;  // Random value 1-6
  int sleepArg2 = rand()%7 + 2;  // Random value 2-8

  // CREATE THREAD1 WITH ARGUMENT:
  // Fourth parameter is the argument to pass
  //
  // PARAMETERS:
  // 1. &thread1: Where to store thread handle
  // 2. NULL: Default attributes
  // 3. func1: Function to execute
  // 4. &sleepArg1: Argument to pass to func1
  //
  // THE ARGUMENT: &sleepArg1
  // We pass the ADDRESS of sleepArg1
  // Why address, not value?
  //
  // REASON:
  // pthread_create signature requires void*
  // void* is a POINTER type
  // Must pass address (pointer) not value
  //
  // WHAT HAPPENS:
  // 1. pthread_create converts &sleepArg1 to void*
  // 2. Passes this void* to func1 as arg parameter
  // 3. func1 casts back to int* and dereferences
  // 4. func1 gets the integer value
  pthread_create(&thread1,NULL,func1,&sleepArg1);

  // CREATE THREAD2 WITH DIFFERENT ARGUMENT:
  // Same pattern, but with different sleep duration
  // thread2 will sleep for sleepArg2 seconds
  pthread_create(&thread2,NULL,func1,&sleepArg2);

  // WAIT FOR THREADS TO COMPLETE:
  // Don't care about return values (threads return NULL)
  // So we pass NULL as second parameter
  //
  // TIMING:
  // These joins happen in order: thread1, then thread2
  // But threads might FINISH in different order
  //
  // EXAMPLE SCENARIO:
  // sleepArg1 = 5, sleepArg2 = 2
  // thread2 finishes at T2, thread1 finishes at T5
  // pthread_join(thread1) waits until T5
  // pthread_join(thread2) returns immediately (already done)
  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);

  return 0;
}

// EXPECTED OUTPUT (example):
// [Yawn] I slept for 3 seconds
// [Yawn] I slept for 5 seconds
//
// OR (depending on random values):
// [Yawn] I slept for 2 seconds
// [Yawn] I slept for 4 seconds
//
// Order depends on which thread finishes first!

// EXECUTION TIMELINE (example):
//
// sleepArg1 = 4, sleepArg2 = 2
//
// Time  Main Thread                  Thread1                    Thread2
// ----  -----------                  -------                    -------
// T0    srand(time(NULL))
// T1    sleepArg1 = 4
// T2    sleepArg2 = 2
// T3    pthread_create(&thread1)
// T4                                 sleepAmount = 4
// T5    pthread_create(&thread2)     sleep(4) begins
// T6                                                            sleepAmount = 2
// T7    pthread_join(thread1) BLOCKS                            sleep(2) begins
// T8                                                            sleep(2) ends
// T9                                                            printf("... 2 seconds")
// T10                                                           return NULL
// T11                                                           (thread2 exits)
// T12   (still blocked)              sleep(4) continues
// T13                                sleep(4) ends
// T14                                printf("... 4 seconds")
// T15                                return NULL
// T16                                (thread1 exits)
// T17   pthread_join returns
// T18   pthread_join(thread2) called
// T19   pthread_join returns immediately (thread2 done)
// T20   return 0

// KEY CONCEPTS:
//
// 1. PASSING ARGUMENTS TO THREADS:
//    Use fourth parameter of pthread_create
//    Must pass a POINTER (address)
//    Thread receives it as void*
//    Thread casts back to correct type
//
// 2. VOID* AS GENERIC POINTER:
//    Can point to any type
//    Main passes int* (via &sleepArg1)
//    Automatically converts to void*
//    Thread casts back to int*
//
// 3. THE CAST PATTERN:
//    *(int*)arg means:
//      (int*)arg - cast void* to int*
//      *(int*)arg - dereference to get int value
//    Must cast THEN dereference
//
// 4. MULTIPLE THREADS, DIFFERENT DATA:
//    Each thread gets its own argument
//    sleepArg1 for thread1
//    sleepArg2 for thread2
//    Allows different behavior per thread
//
// 5. STACK VARIABLE SAFETY:
//    Safe here because main() outlives threads
//    Dangerous if creating thread lives longer than variable
//    Better to use heap for long-lived threads

// DETAILED CAST EXPLANATION:
//
// THE EXPRESSION: *(int*)arg
//
// Let's see what's happening step by step:
//
// 1. arg has type void*
//    It's a pointer, but we don't know what it points to
//
// 2. (int*)arg casts void* to int*
//    Now the compiler knows: "this points to an int"
//
// 3. *(int*)arg dereferences the int*
//    Reads the integer value from memory
//
// MEMORY VIEW:
//
// In main:
//   sleepArg1 is at address 0x7fff1234 (example)
//   Value at 0x7fff1234 is: 4
//
// pthread_create call:
//   &sleepArg1 is 0x7fff1234 (address)
//   Converts to void* automatically
//
// In func1:
//   arg is 0x7fff1234 (same address, but type is void*)
//   (int*)arg is 0x7fff1234 (same address, type now int*)
//   *(int*)arg reads memory at 0x7fff1234, gets value: 4

// COMMON MISTAKES:
//
// MISTAKE 1: Forgetting to dereference
//   int sleepAmount = (int*)arg;  // WRONG! Type mismatch
//   Error: cannot convert int* to int
//
// MISTAKE 2: Casting pointer to int instead of dereferencing
//   int sleepAmount = (int)arg;  // WRONG! Casts address to int
//   Result: sleepAmount = 0x7fff1234 (the address!)
//   sleep(0x7fff1234) would sleep for billions of seconds
//
// MISTAKE 3: Wrong cast order
//   int sleepAmount = (int)(arg*);  // SYNTAX ERROR
//   Can't put * after variable name in a cast
//
// CORRECT:
//   int sleepAmount = *(int*)arg;  // RIGHT!
//   Cast first, then dereference

// WHY PASS ADDRESS INSTEAD OF VALUE?
//
// QUESTION: Why not just pass the integer directly?
// ANSWER: pthread_create signature requires void*
//
// YOU CAN'T DO:
//   pthread_create(&thread1, NULL, func1, 42);  // WRONG!
//   Error: cannot convert int to void*
//
// YOU MUST DO:
//   int value = 42;
//   pthread_create(&thread1, NULL, func1, &value);  // CORRECT
//
// HACKY ALTERNATIVE (not recommended):
//   pthread_create(&thread1, NULL, func1, (void*)42);  // Hacky
//   Then in func1: int value = (int)arg;
//   This casts int to pointer (non-portable, confusing)
//
// PROPER PATTERN:
//   Always pass address of data
//   Thread casts and dereferences
//   Clear, portable, safe

// STACK LIFETIME DANGER:
//
// SAFE PATTERN (this file):
//   int main() {
//     int arg = 42;
//     pthread_create(&thread, NULL, func, &arg);
//     pthread_join(thread, NULL);  // arg still valid
//   }
//
// DANGEROUS PATTERN (don't do this!):
//   void create_thread() {
//     int arg = 42;
//     pthread_create(&thread, NULL, func, &arg);
//     // Function returns, arg is destroyed
//     // But thread might still be running!
//     // Thread accesses garbage memory
//   }
//
// SOLUTION FOR LONG-LIVED THREADS:
//   int* arg = malloc(sizeof(int));
//   *arg = 42;
//   pthread_create(&thread, NULL, func, arg);
//   // Thread must free(arg) when done
//   // Or mark thread as detached

// COMPARISON TO PREVIOUS EXAMPLES:
//
// threads1.c: No arguments, no return
//   pthread_create(&thread, NULL, func, NULL);
//   pthread_join(thread, NULL);
//
// threads2.c: No arguments, capture return (but it's NULL)
//   pthread_create(&thread, NULL, func, NULL);
//   pthread_join(thread, (void**)&ret);
//
// threads3.c: No arguments, meaningful return
//   pthread_create(&thread, NULL, func, NULL);
//   pthread_join(thread, (void**)&ret);
//
// threads4.c (this file): Arguments, no return
//   pthread_create(&thread, NULL, func, &arg);
//   pthread_join(thread, NULL);
//
// COMPLETE PATTERN (both args and return):
//   int* arg = ...;
//   int* ret;
//   pthread_create(&thread, NULL, func, arg);
//   pthread_join(thread, (void**)&ret);

// COMPILE AND RUN:
// gcc threads4.c -o threads4 -pthread
// ./threads4
//
// Run multiple times:
// Observe different sleep durations
// Observe messages in different orders
//
// SAMPLE OUTPUTS:
//
// Run 1:
//   [Yawn] I slept for 2 seconds
//   [Yawn] I slept for 5 seconds
//
// Run 2:
//   [Yawn] I slept for 4 seconds
//   [Yawn] I slept for 3 seconds
//
// Run 3:
//   [Yawn] I slept for 6 seconds
//   [Yawn] I slept for 7 seconds

// TRY IT:
// 1. Compile and run several times
//    Observe different random durations
//    Notice shorter sleep prints first
//
// 2. Change to pass same variable to both threads:
//    pthread_create(&thread1, NULL, func1, &sleepArg1);
//    pthread_create(&thread2, NULL, func1, &sleepArg1);
//    Both threads sleep same duration
//
// 3. Try the wrong cast:
//    int sleepAmount = (int)arg;
//    See what happens (very long sleep!)
//
// 4. Try passing NULL:
//    pthread_create(&thread1, NULL, func1, NULL);
//    What happens? (Crash when dereferencing NULL)
//
// 5. Create a struct with multiple fields:
//    struct ThreadArgs { int sleep; char* msg; };
//    Pass &args, cast to (struct ThreadArgs*)arg
//    Access multiple pieces of data

// PRACTICAL USES:
//
// WORKER POOL:
//   for (int i = 0; i < 10; i++) {
//     int* id = malloc(sizeof(int));
//     *id = i;
//     pthread_create(&threads[i], NULL, worker, id);
//   }
//   Each thread knows its ID
//
// DIFFERENT TASKS:
//   struct Task { int type; void* data; };
//   Pass task to thread, thread executes based on type
//
// CONFIGURATION:
//   struct Config { int timeout; char* url; };
//   Pass config to thread for customized behavior

// NEXT STEPS:
// Now you know how to:
//   - Create threads
//   - Pass data TO threads
//   - Return data FROM threads
//   - Wait for threads
//
// Next topics:
//   - Mutexes (protecting shared data)
//   - Condition variables (thread coordination)
//   - Detached threads (fire and forget)
//   - Thread pools (reusing threads)
