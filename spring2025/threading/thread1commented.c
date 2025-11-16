// PEDAGOGICAL PURPOSE:
// This program introduces POSIX threads (pthreads) - a lightweight alternative to processes.
// Key learning objectives:
// 1. Understanding threads vs processes: threads SHARE memory, processes don't
// 2. How to create threads with pthread_create()
// 3. How to wait for threads with pthread_join()
// 4. Thread function signature: void* func(void* arg)
// 5. Why void* pointers enable generic thread functions
// 6. Passing arguments to threads
// 7. Concurrent execution within a single process

#include <stdio.h>
#include <pthread.h>     // POSIX threads library
#include <unistd.h>      // For sleep() and other POSIX functions
#include <time.h>        // For time functions (not used here, but commonly needed)
#include <stdlib.h>      // Standard library

/*
 * THREADS vs PROCESSES:
 *
 * PROCESSES (fork):
 * - Separate memory spaces
 * - More isolated and secure
 * - Heavier weight (more overhead)
 * - Communication requires IPC (pipes, sockets, etc.)
 *
 * THREADS (pthread_create):
 * - SHARED memory space within a process
 * - Lighter weight (less overhead)
 * - Easier communication (just use shared variables)
 * - Need synchronization to prevent conflicts
 * - Less isolated (one thread crash can affect all threads)
 *
 * WHEN TO USE THREADS?
 * - When tasks need to share data frequently
 * - When you need many concurrent tasks (threads are cheaper)
 * - When performance is critical (less context-switch overhead)
 */

/*
 * Thread function - this is what each thread will execute
 *
 * TEACHING POINT: Thread functions MUST have this exact signature:
 * void* function_name(void* arg)
 *
 * WHY THIS SIGNATURE?
 * - void* arg: Generic pointer can receive ANY type of data
 * - void* return: Can return ANY type of data
 * This makes threads flexible - they can work with any data type!
 *
 * The comments in the original code explain this well:
 * "note that our thread function is always going to have this type
 *  it takes in a void* and returns a void*
 *  why?
 *  void* is kind of the wildcard pointer
 *  remember how we can cast pointers to basically anything?
 *  well that means a void* pointer is kind of the most generic pointer
 *  C lets you implicitly cast to void*"
 */
void* ourPrinter(void* arg){
  /*
   * Cast the void* argument to the expected type (char*)
   *
   * TEACHING POINT: We know we're passing a string (char*) to this
   * function, so we cast the generic void* to char* to use it.
   *
   * This is a common pattern:
   * 1. Receive void* argument
   * 2. Cast to actual type
   * 3. Use the typed pointer
   *
   * WHY NOT JUST USE char* AS THE PARAMETER TYPE?
   * Because pthread_create() requires the function signature to be
   * void* (*)(void*). All thread functions must have this signature.
   */
  char* msg = (char*) arg;

  /*
   * Print the message that was passed to this thread
   *
   * TEACHING POINT: Each thread that calls this function will
   * receive a different argument (msg1 vs msg2) and print a
   * different message.
   */
  printf("Our thread says: %s", msg);

  /*
   * Return NULL (no result to return)
   *
   * TEACHING POINT: Thread functions must return void*
   * Even if we have nothing useful to return, we return NULL.
   *
   * The original code notes:
   * "return 0; <--- ookay, but a little naughty
   *  0 IS NULL but it's visually unhelpful"
   *
   * WHY RETURN NULL INSTEAD OF 0?
   * - NULL is more semantically clear (it's a pointer)
   * - 0 works (it's implicitly converted to NULL)
   * - But NULL better expresses intent
   */
  return NULL;
}

int main(){
  /*
   * Declare thread identifiers
   *
   * TEACHING POINT: pthread_t is an opaque type that uniquely
   * identifies a thread. Think of it like pid_t for processes.
   *
   * We need these to:
   * 1. Create threads (pthread_create fills these in)
   * 2. Wait for threads (pthread_join uses these)
   * 3. Manage threads (send signals, cancel, etc.)
   */
  pthread_t thread1;
  pthread_t thread2;

  /*
   * Prepare messages to pass to each thread
   *
   * TEACHING POINT: These strings are in main's stack memory.
   * But since threads SHARE memory space, both threads can
   * access these strings!
   *
   * This is a key difference from processes:
   * - With fork(): each process gets its own copy of variables
   * - With threads: all threads share the same variables
   */
  char* msg1 = "Hi there I'm one thread\n";
  char* msg2 = "Hi there I'm a different thread\n";

  /*
   * Create the first thread
   *
   * TEACHING POINT: pthread_create() creates and starts a new thread.
   *
   * Arguments:
   * 1. &thread1: Address to store the thread identifier
   * 2. NULL: Thread attributes (NULL = use defaults)
   * 3. ourPrinter: Function the thread will execute
   * 4. msg1: Argument to pass to the thread function
   *
   * What happens:
   * - A new thread is created
   * - The new thread starts executing ourPrinter(msg1)
   * - The main thread continues to the next line
   * - Both threads now run CONCURRENTLY
   */
  pthread_create(&thread1,NULL,ourPrinter,msg1);

  /*
   * Create the second thread
   *
   * This thread also executes ourPrinter, but with msg2 as the argument.
   *
   * Now we have THREE threads running:
   * 1. Main thread (executing this code)
   * 2. Thread1 (executing ourPrinter with msg1)
   * 3. Thread2 (executing ourPrinter with msg2)
   */
  pthread_create(&thread2,NULL,ourPrinter,msg2);

  /*
   * Wait for thread1 to finish
   *
   * TEACHING POINT: pthread_join() is like wait() for threads.
   * It blocks the calling thread until the specified thread terminates.
   *
   * Arguments:
   * 1. thread1: Which thread to wait for
   * 2. NULL: Where to store the thread's return value (NULL = don't care)
   *
   * WHY JOIN?
   * If main() returns without joining, the entire process exits,
   * killing all threads! We must join to ensure threads complete.
   */
  pthread_join(thread1,NULL);

  /*
   * Wait for thread2 to finish
   *
   * After both joins, we know both threads have completed.
   */
  pthread_join(thread2,NULL);

  return 0;
  // All threads have finished, safe to exit
}

// EXECUTION TRACE:
//
// Time  Thread     Action
// ----  ------     ------
// T0    Main       Creates thread1
// T1    Main       Creates thread2
//       Thread1    Starts executing ourPrinter(msg1)
//       Thread2    Starts executing ourPrinter(msg2)
// T2    Main       Calls pthread_join(thread1), BLOCKS
//       Thread1    Prints "Hi there I'm one thread\n"
//       Thread2    Prints "Hi there I'm a different thread\n"
// T3    Thread1    Returns NULL, exits
// T4    Main       pthread_join returns (thread1 done)
// T5    Main       Calls pthread_join(thread2), BLOCKS
// T6    Thread2    Returns NULL, exits
// T7    Main       pthread_join returns (thread2 done)
// T8    Main       Returns 0, program exits

// EXPECTED OUTPUT (order may vary):
// Hi there I'm one thread
// Hi there I'm a different thread
//
// OR:
// Hi there I'm a different thread
// Hi there I'm one thread
//
// WHY MIGHT ORDER VARY?
// The OS scheduler decides when each thread runs. Thread1 might
// print first, or thread2 might print first - it's non-deterministic!

// TRY IT:
// Compile: gcc thread1.c -o thread1 -lpthread
// Note: -lpthread links the pthread library
// Run: ./thread1
// Run it multiple times - the order might change!
//
// EXPERIMENT:
// 1. Create more threads (thread3, thread4, etc.)
// 2. Add sleep() calls in ourPrinter to make timing visible
// 3. Try passing different types of data (not just strings)
//
// NEXT STEPS:
// This example shows basic thread creation and joining.
// But threads share memory - what happens when they access
// the same variables? See threadRace1.c to find out!
