// PEDAGOGICAL PURPOSE:
// This program demonstrates how threads can return values to their caller using pthread_join().
// Key learning objectives:
// 1. How to return data from a thread function
// 2. Capturing thread return values with pthread_join()
// 3. Memory management for thread return values (heap allocation required!)
// 4. Why we can't return pointers to local/stack variables from threads
// 5. Type casting with void* for generic returns
// 6. Proper cleanup - freeing memory allocated by threads
// 7. Thread communication through return values

#include <stdio.h>
#include <pthread.h>     // POSIX threads
#include <unistd.h>      // For sleep()
#include <time.h>        // For time() - seeding random
#include <stdlib.h>      // For malloc(), free(), rand(), srand()

/*
 * Thread function that returns a value
 *
 * TEACHING POINT: This function demonstrates how to return data from a thread.
 * The challenge: thread functions return void*, but we want to return an int.
 *
 * SOLUTION: Allocate memory on the heap and return a pointer to it.
 *
 * WHY NOT RETURN A LOCAL VARIABLE?
 * Local variables are on the stack and disappear when the function returns.
 * Returning a pointer to a local variable = DANGLING POINTER = BUG!
 *
 * Example of what NOT to do:
 * int sleepTime = rand()%5+1;
 * return &sleepTime;  // BAD! sleepTime disappears when function returns!
 */
void* weirdFunction(void* arg){

  /*
   * Allocate memory on the HEAP for the return value
   *
   * TEACHING POINT: malloc() allocates memory that persists even after
   * this function returns. This is how we can return data to the caller.
   *
   * The allocated memory will hold one integer (the sleep duration).
   *
   * IMPORTANT: Whoever receives this pointer is responsible for freeing it!
   * This is a common pattern in C - caller must clean up.
   */
  int* sleepPointer = malloc(sizeof(int));

  /*
   * Store a random sleep time (1-6 seconds)
   *
   * TEACHING POINT: We're dereferencing sleepPointer to store the value
   * in the allocated memory.
   */
  *sleepPointer = rand()%5+1;

  /*
   * Sleep for that many seconds
   *
   * TEACHING POINT: This simulates work. Each thread will sleep for
   * a different random duration, then return how long it slept.
   */
  sleep(*sleepPointer);

  /*
   * Return the pointer to the allocated memory
   *
   * TEACHING POINT: We return sleepPointer (which is a void*).
   * Actually, sleepPointer is an int*, but C allows implicit conversion
   * to void* for return statements.
   *
   * The caller can retrieve this pointer using pthread_join().
   */
  return sleepPointer;
}

int main(){

  /*
   * Seed random number generator
   * Using time(0) ensures different random values each run
   */
  srand(time(0));

  /*
   * Create thread identifiers
   */
  pthread_t thread1;
  pthread_t thread2;

  /*
   * Variables to store thread return values
   *
   * TEACHING POINT: These are void* because pthread_join() returns void*.
   * We'll cast them to int* when we use them.
   */
  void* res1;
  void* res2;

  /*
   * Create two threads
   *
   * Both execute weirdFunction(), which will:
   * 1. Generate a random sleep time
   * 2. Sleep for that duration
   * 3. Return the sleep time
   */
  pthread_create(&thread1,NULL,weirdFunction,NULL);
  pthread_create(&thread2,NULL,weirdFunction,NULL);

  /*
   * Wait for thread1 and capture its return value
   *
   * TEACHING POINT: The second argument to pthread_join() is where the
   * return value is stored.
   *
   * &res1 is a void** (pointer to void*)
   * pthread_join() will store the thread's return value there
   *
   * After this call:
   * - thread1 has finished
   * - res1 points to the memory allocated by thread1 (containing sleep time)
   */
  pthread_join(thread1,&res1);

  /*
   * Wait for thread2 and capture its return value
   */
  pthread_join(thread2,&res2);

  /*
   * Print the results
   *
   * TEACHING POINT: The type casting here is complex but important!
   *
   * *(int*)res1 breaks down as:
   * 1. res1: void* pointing to the allocated memory
   * 2. (int*)res1: cast void* to int* (now compiler knows it points to int)
   * 3. *(int*)res1: dereference to get the actual int value
   *
   * This is the reverse of what the thread did:
   * Thread: allocated int, returned int* (as void*)
   * Main: receives void*, casts to int*, dereferences to get int
   */
  printf("Thread 1 did a sleep for %d seconds\n",*(int*)res1);
  printf("Thread 2 did a sleep for %d seconds\n",*(int*)res2);

  /*
   * Free the memory allocated by the threads
   *
   * TEACHING POINT: This is critical! The threads allocated memory with
   * malloc(), so we must free() it to avoid memory leaks.
   *
   * We cast to (int*) before freeing. While free() accepts any pointer,
   * this makes our intent clearer.
   *
   * MEMORY MANAGEMENT RULE:
   * - Thread allocates with malloc()
   * - Thread returns pointer
   * - Main receives pointer
   * - Main frees memory
   *
   * This is a common pattern: allocator and freer can be different!
   */
  free((int*)res1);
  free((int*)res2);

  return 0;
}

// EXECUTION TRACE:
//
// Time  Thread    Action                              Memory
// ----  ------    ------                              ------
// T0    Main      Creates thread1, thread2            -
// T1    Thread1   malloc() -> 0x1000                  [0x1000: ?]
//       Thread2   malloc() -> 0x2000                  [0x2000: ?]
// T2    Thread1   *0x1000 = 3 (random)                [0x1000: 3]
//       Thread2   *0x2000 = 5 (random)                [0x2000: 5]
// T3    Thread1   sleep(3)...                         [0x1000: 3]
//       Thread2   sleep(5)...                         [0x2000: 5]
// T4    Main      pthread_join(thread1), blocks       -
// T5    Thread1   (wakes after 3 sec)                 [0x1000: 3]
// T6    Thread1   returns 0x1000                      [0x1000: 3]
// T7    Main      res1 = 0x1000, join returns         [0x1000: 3]
// T8    Main      pthread_join(thread2), blocks       -
// T9    Thread2   (wakes after 5 sec)                 [0x2000: 5]
// T10   Thread2   returns 0x2000                      [0x2000: 5]
// T11   Main      res2 = 0x2000, join returns         [0x2000: 5]
// T12   Main      Prints "Thread 1... 3 seconds"      [0x1000: 3]
// T13   Main      Prints "Thread 2... 5 seconds"      [0x2000: 5]
// T14   Main      free(0x1000), free(0x2000)          [memory freed]
// T15   Main      Exits                               -

// EXPECTED OUTPUT (values will vary):
// Thread 1 did a sleep for 3 seconds
// Thread 2 did a sleep for 5 seconds
//
// Each run will have different random sleep times (1-6 seconds)

// TRY IT:
// Compile: gcc threadResult.c -o threadResult -lpthread
// Run: ./threadResult
// Notice it takes a few seconds to complete (threads are sleeping)
// Run multiple times - the sleep durations will vary
//
// EXPERIMENT:
// 1. What happens if you forget to free() the memory?
//    Run with valgrind: valgrind ./threadResult
//    You'll see memory leaks!
//
// 2. What happens if you try to return a local variable?
//    Try this BAD version:
//    void* weirdFunction(void* arg) {
//        int sleepTime = rand()%5+1;
//        sleep(sleepTime);
//        return &sleepTime;  // BUG!
//    }
//    This will compile but give garbage values!
//
// 3. Create more threads and collect all their results in an array
//
// COMMON PATTERNS FOR THREAD RETURN VALUES:
//
// 1. Simple value (this example):
//    - Allocate small piece of data on heap
//    - Return pointer
//    - Caller frees
//
// 2. Complex structure:
//    struct Result {
//        int status;
//        char* message;
//    };
//    struct Result* r = malloc(sizeof(struct Result));
//    return r;
//
// 3. No return value:
//    - Return NULL
//    - Use shared memory for communication instead
//
// KEY TAKEAWAYS:
// - Threads can return data via their return value
// - Must use heap allocation (malloc) for returned data
// - Caller is responsible for freeing the memory
// - pthread_join() is how we retrieve the return value
// - Type casting with void* enables generic returns
