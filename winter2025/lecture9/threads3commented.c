// PEDAGOGICAL PURPOSE:
// This program demonstrates returning actual data from threads (not just NULL).
// It shows how threads can compute results and return them to the main thread.
// Key learning objectives:
// 1. Understanding how to return meaningful data from threads
// 2. Learning why malloc is necessary for thread return values
// 3. Understanding memory ownership transfer from thread to main
// 4. Learning proper memory management with threaded returns
// 5. Seeing practical use of pthread_join's return value parameter
// 6. Understanding why stack variables can't be returned from threads
// 7. Learning to dereference returned pointers to access data
// 8. Observing non-deterministic thread completion times

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

// COMMENT FROM ORIGINAL CODE:
// "when you see void*, that's polymorphism in C"
//
// In this example, void* allows us to return int* from thread
// Even though signature says void*, we cast int* to void*

// THREAD FUNCTION:
// This thread computes a result and returns it
//
// WHAT'S NEW:
// Instead of returning NULL, we return a POINTER TO DATA
// That data is allocated with malloc
// The main thread will receive and use this data
void* func1(void* arg){

  // ALLOCATE MEMORY FOR RETURN VALUE:
  // malloc allocates memory on the HEAP
  //
  // WHY MALLOC?
  // We're going to RETURN this pointer
  // Thread function will END, so its stack disappears
  // Stack variables are INVALID after function returns
  //
  // WHAT IF WE USED STACK INSTEAD? (WRONG!)
  //   int localSleep = rand()%5+1;
  //   return &localSleep;  // DISASTER!
  // After func1 returns, localSleep is GONE
  // Pointer points to garbage (undefined behavior)
  //
  // MALLOC SOLUTION:
  // Heap memory persists until explicitly freed
  // Safe to return pointer to heap memory
  // Main thread can access it after thread ends
  int* sleepPointer = malloc(sizeof(int));

  // COMPUTE A RANDOM VALUE:
  // Random sleep time between 1 and 5 seconds
  // We'll return this value to demonstrate data transfer
  //
  // WHY RANDOM?
  // Makes thread execution non-deterministic
  // Simulates threads taking variable time
  // More realistic than fixed delays
  *sleepPointer = rand()%5+1;

  // SLEEP FOR THAT DURATION:
  // Thread sleeps for the random duration
  // This simulates "work" being done
  // Different threads will finish at different times
  //
  // TIMING NOTE:
  // Thread1 might sleep 3 seconds
  // Thread2 might sleep 1 second
  // Thread2 will finish first!
  // But we join in order (thread1 then thread2)
  // So main() might wait longer for thread1
  sleep(*sleepPointer);

  // RETURN THE POINTER:
  // Cast int* to void* (required by signature)
  // Main thread will receive this pointer
  // Main thread OWNS this memory now (must free it!)
  //
  // OWNERSHIP TRANSFER:
  // Thread allocates (malloc)
  // Thread returns ownership to main
  // Main must eventually free
  return sleepPointer;
}

int main(){

  // SEED RANDOM NUMBER GENERATOR:
  // Different seed each run = different sleep times
  srand(time(NULL));

  // THREAD HANDLES:
  pthread_t thread1;
  pthread_t thread2;

  // RETURN VALUE POINTERS:
  // These will receive the malloc'd integers from threads
  // After pthread_join, these will point to valid heap memory
  // We're responsible for freeing this memory!
  int* ret1;
  int* ret2;

  // CREATE THREADS:
  // Both threads execute func1
  // Each will malloc an int, compute a value, sleep, and return
  pthread_create(&thread1,NULL,func1,NULL);
  pthread_create(&thread2,NULL,func1,NULL);

  // JOIN THREAD1 AND GET RETURN VALUE:
  // Block until thread1 finishes
  // Receive the pointer it returns
  // Store it in ret1
  //
  // WHAT pthread_join DOES:
  // 1. Wait for thread1 to finish
  // 2. Get the return value (void*) from func1
  // 3. Store it at address &ret1
  // 4. ret1 now points to the malloc'd int
  //
  // TIMING:
  // If thread1 sleeps 4 seconds, we wait 4 seconds here
  // Even if thread2 finishes first, we still wait for thread1
  pthread_join(thread1,(void**)&ret1);

  // JOIN THREAD2 AND GET RETURN VALUE:
  // Same as above for thread2
  //
  // TIMING NOTE:
  // If thread2 already finished (sleep was shorter), this returns immediately
  // If thread2 still running (sleep is longer), we wait here
  pthread_join(thread2,(void**)&ret2);

  // PRINT THE RESULTS:
  // Dereference the pointers to get the actual integers
  // %d is format specifier for integers
  //
  // WHAT WE'RE PRINTING:
  // The random sleep duration each thread used
  // This is the data the thread computed and returned
  //
  // EXPECTED OUTPUT:
  // The value of ret1 is: 3   (or 1-5, random)
  // The value of ret2 is: 2   (or 1-5, random)
  printf("The value of ret1 is: %d\n",*ret1);
  printf("The value of ret2 is: %d\n",*ret2);

  // MEMORY CLEANUP:
  // We must free the memory that threads allocated!
  //
  // WHY NECESSARY?
  // Threads called malloc, but they can't call free (they're gone)
  // Main thread now OWNS this memory
  // If we don't free: MEMORY LEAK
  //
  // OWNERSHIP RULES:
  // Whoever allocates isn't always who frees
  // Whoever OWNS must free
  // Thread transferred ownership by returning pointer
  //
  // MISSING IN THIS CODE!
  // This program has a memory leak
  // Should add:
  //   free(ret1);
  //   free(ret2);

  return 0;
}

// EXPECTED OUTPUT:
// The value of ret1 is: 3
// The value of ret2 is: 5
//
// (Numbers will vary each run due to rand())

// EXECUTION TIMELINE (example):
//
// Time  Main Thread                  Thread1                    Thread2
// ----  -----------                  -------                    -------
// T0    srand(time(NULL))
// T1    pthread_create(&thread1)
// T2                                 malloc sleepPointer
// T3    pthread_create(&thread2)     *sleepPointer = 3
// T4                                 sleep(3) begins            malloc sleepPointer
// T5    pthread_join(thread1) BLOCKS                            *sleepPointer = 1
// T6                                                             sleep(1) begins
// T7                                                             sleep(1) ends
// T8                                                             return pointer
// T9                                                             (thread2 exits)
// T10   (still blocked, waiting)     sleep(3) continues
// T11                                sleep(3) ends
// T12                                return pointer
// T13                                (thread1 exits)
// T14   pthread_join returns
// T15   ret1 = pointer from thread1
// T16   pthread_join(thread2) called
// T17   pthread_join returns immediately (thread2 already done)
// T18   ret2 = pointer from thread2
// T19   printf(*ret1) → 3
// T20   printf(*ret2) → 1
// T21   return 0 (MEMORY LEAK - didn't free ret1, ret2!)

// KEY CONCEPTS:
//
// 1. RETURNING DATA FROM THREADS:
//    Can't return stack variables (they disappear)
//    Must use heap memory (malloc)
//    Return pointer to heap data
//    Cast to void* when returning
//
// 2. MEMORY OWNERSHIP:
//    Thread allocates (malloc)
//    Thread transfers ownership (return)
//    Main receives ownership (pthread_join)
//    Main must free (or memory leak)
//
// 3. HEAP vs STACK:
//    STACK:
//      - Automatic storage
//      - Disappears when function returns
//      - Cannot return pointers to stack variables
//      - Fast allocation
//
//    HEAP:
//      - Manual storage (malloc/free)
//      - Persists until freed
//      - Can return pointers to heap memory
//      - Slower allocation
//
// 4. DEREFERENCING RETURN VALUES:
//    pthread_join gives us a POINTER
//    Must dereference to get actual value
//    *ret1 gets the int value
//    ret1 itself is just an address

// WHY NOT RETURN INT DIRECTLY?
//
// CAN'T DO THIS:
//   int func1(void* arg) {    // Wrong signature!
//     return 42;
//   }
//
// MUST USE POINTER:
//   void* func1(void* arg) {   // Correct signature
//     int* result = malloc(sizeof(int));
//     *result = 42;
//     return result;
//   }
//
// REASON:
// pthread_create requires signature: void* (*)(void*)
// Can't change return type
// Must work within this constraint
//
// ALTERNATIVE (hacky, not recommended):
//   return (void*)42;   // Cast int to pointer
//   int value = (int)ret1;  // Cast pointer to int
// This works for small integers but is non-portable and confusing

// MEMORY LEAK ANALYSIS:
//
// This program leaks memory!
// Each thread allocates sizeof(int) bytes
// Two threads = 2 * sizeof(int) = 8 bytes typically
//
// SMALL LEAK:
// Only 8 bytes, program exits immediately
// OS reclaims all memory when process terminates
// Not a problem for this demo
//
// LARGE LEAK IF SCALED:
// If we created 1000 threads, leak is 8000 bytes
// If we loop and create threads repeatedly, leak grows
// Would eventually run out of memory
//
// PROPER CLEANUP:
// Add before return 0:
//   free(ret1);
//   free(ret2);

// STACK VARIABLE DANGER (DEMONSTRATION):
//
// WRONG VERSION (don't do this!):
//   void* func1(void* arg) {
//     int sleepValue = rand()%5+1;  // Stack variable
//     sleep(sleepValue);
//     return &sleepValue;  // DANGER! Returning address of local variable
//   }
//
// WHY IT FAILS:
// - sleepValue lives on func1's stack
// - When func1 returns, its stack frame is destroyed
// - &sleepValue points to INVALID memory
// - Accessing it is undefined behavior
// - Might work by accident, might crash, might return garbage
//
// COMPILER WARNING:
// gcc will warn: "function returns address of local variable"
// ALWAYS HEED THIS WARNING!

// PROPER PATTERN FOR THREAD RETURN VALUES:
//
// 1. Allocate heap memory (malloc)
// 2. Store result in heap memory
// 3. Return pointer to heap memory
// 4. Main thread captures pointer (pthread_join)
// 5. Main thread uses data
// 6. Main thread frees memory

// COMPARE TO THREADS2.C:
//
// threads2.c:
//   - Returns NULL (no data)
//   - Demonstrates mechanism
//   - No memory management needed
//
// threads3.c (this file):
//   - Returns actual data (int)
//   - Demonstrates practical use
//   - Requires malloc/free

// COMPILE AND RUN:
// gcc threads3.c -o threads3 -pthread
// ./threads3
//
// Run multiple times:
// Observe different random values each time
//
// SAMPLE OUTPUTS:
//
// Run 1:
//   The value of ret1 is: 2
//   The value of ret2 is: 4
//
// Run 2:
//   The value of ret1 is: 5
//   The value of ret2 is: 1
//
// Run 3:
//   The value of ret1 is: 3
//   The value of ret2 is: 3

// TRY IT:
// 1. Compile and run several times
//    Observe different random values
//    Notice program takes different amounts of time
//
// 2. Add free(ret1) and free(ret2) before return 0
//    Check with valgrind: valgrind ./threads3
//    Should show no leaks after fix
//
// 3. Change to return &localVariable (stack variable)
//    See compiler warning
//    Run anyway - might crash or show garbage
//
// 4. Print timestamp before/after pthread_join
//    See that we wait for thread1 (even if thread2 finished first)
//
// 5. Try returning (void*)42 instead of malloc'd pointer
//    Then print (int)ret1 instead of *ret1
//    Observe it works but is confusing (casting int to pointer)

// NEXT STEP:
// threads4.c shows how to PASS data TO threads (not just return)
// Completes the picture: data in, processing, data out
