// PEDAGOGICAL PURPOSE:
// This program introduces the absolute basics of POSIX threads (pthreads).
// It demonstrates creating multiple threads that execute the same function.
// Key learning objectives:
// 1. Understanding basic pthread_create() usage
// 2. Learning the required thread function signature: void* (*)(void*)
// 3. Understanding pthread_join() for waiting on thread completion
// 4. Seeing how multiple threads can execute the same function
// 5. Understanding void* as C's polymorphism mechanism
// 6. Learning why function pointers are necessary for threads
// 7. Observing concurrent execution of threads
// 8. Understanding the pthread_t type for thread handles

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

// COMMENT FROM ORIGINAL CODE:
// "when you see void*, that's polymorphism in C"
//
// WHAT IS void*?
// void* is a "generic pointer" - it can point to ANY type
//   - Can assign any pointer to void*
//   - Can cast void* to any pointer type
//   - This is C's way of achieving type flexibility
//
// WHY void* FOR THREADS?
// pthread_create needs to work with ANY type of function
// Different threads might:
//   - Process integers
//   - Process strings
//   - Process structs
//   - Take no parameters
//
// The solution: void*!
// Every thread function has the SAME signature
// But inside, we can work with any type we need

// THREAD FUNCTION:
// This is the function that each thread will execute
//
// REQUIRED SIGNATURE:
// void* functionName(void* arg)
//   - MUST return void* (even if returning NULL)
//   - MUST take void* parameter (even if not using it)
//
// This signature is MANDATED by pthread_create
// You cannot use a different signature
//
// WHAT THIS FUNCTION DOES:
// Simple: just prints a message
// Both threads will execute this same function
void* func1(void* arg){

  // SIMPLE OUTPUT:
  // Print a message to show this thread is running
  // When you run this program, you'll see this message twice
  // (once from each thread)
  printf("Hi I'm a thread!\n");

  // RETURN NULL:
  // We must return void* (required by signature)
  // NULL is a valid void* value meaning "no data"
  //
  // In this simple example, we don't have useful data to return
  // Later examples will return actual values via this mechanism
  return NULL;
}

int main(){

  // THREAD HANDLES:
  // pthread_t is an opaque type representing a thread
  // "Opaque" means we don't know (or care) what's inside
  // It's a HANDLE - a reference to a thread
  //
  // ANALOGY:
  // Like a ticket stub at a coat check
  // You give it back to pthread_join to "claim" the thread
  //
  // We're creating TWO threads, so we need TWO handles
  pthread_t thread1;
  pthread_t thread2;

  // CREATE FIRST THREAD:
  // pthread_create spawns a new thread
  //
  // FUNCTION SIGNATURE:
  // int pthread_create(
  //   pthread_t *thread,              // Where to store thread handle
  //   const pthread_attr_t *attr,     // Thread attributes (NULL = default)
  //   void *(*start_routine)(void*),  // Function to run (FUNCTION POINTER!)
  //   void *arg                       // Argument to pass to function
  // )
  //
  // PARAMETERS EXPLAINED:
  // &thread1: Address where pthread_create will store the thread ID
  //           pthread_create fills this in (it's an OUTPUT parameter)
  //
  // NULL: Use default attributes
  //       Default = normal priority, default stack size, joinable
  //       Advanced uses: set stack size, scheduling policy, etc.
  //
  // func1: The function for the thread to execute
  //        NOTE: This is a FUNCTION POINTER!
  //        We're passing the ADDRESS of the function
  //        This is WHY we learned about function pointers!
  //
  // NULL: We're not passing any argument to func1
  //       func1's arg parameter will receive NULL
  //
  // WHAT HAPPENS:
  // 1. New thread is created
  // 2. New thread starts executing func1(NULL)
  // 3. pthread_create returns IMMEDIATELY (doesn't wait)
  // 4. Main thread continues to next line
  // 5. Both threads now run CONCURRENTLY
  pthread_create(&thread1,NULL,func1,NULL);

  // CREATE SECOND THREAD:
  // Same process as above
  // Now we have THREE threads total:
  //   - Main thread (original)
  //   - thread1 (just created)
  //   - thread2 (about to create)
  //
  // All three run CONCURRENTLY
  pthread_create(&thread2,NULL,func1,NULL);

  // WAIT FOR FIRST THREAD:
  // pthread_join blocks until the specified thread terminates
  //
  // FUNCTION SIGNATURE:
  // int pthread_join(
  //   pthread_t thread,    // The thread to wait for
  //   void **retval        // Where to store the thread's return value
  // )
  //
  // PARAMETERS EXPLAINED:
  // thread1: The thread handle from pthread_create
  //          We're waiting for thread1 to finish
  //
  // NULL: We don't care about the return value
  //       func1 returns NULL anyway, so no useful data
  //       If we wanted the return value, we'd pass &some_pointer
  //
  // ANALOGY TO PROCESSES:
  // This is like wait() for processes:
  //   - Parent process calls wait() to wait for child
  //   - Main thread calls pthread_join() to wait for spawned thread
  //
  // WHY NECESSARY?
  // Without pthread_join:
  //   - main() might exit before threads finish
  //   - When main() exits, entire process terminates
  //   - All threads are killed immediately
  //   - Threads might not get a chance to print!
  //
  // With pthread_join:
  //   - main() waits until thread completes
  //   - Ensures thread finishes its work
  //   - Clean, orderly shutdown
  pthread_join(thread1,NULL);

  // WAIT FOR SECOND THREAD:
  // Same as above, but waiting for thread2
  // Main thread now waits for BOTH spawned threads to complete
  pthread_join(thread2,NULL);

  // AT THIS POINT:
  // - Both thread1 and thread2 have completed
  // - Both have printed their messages
  // - Safe to exit

  return 0;
}

// EXPECTED OUTPUT:
// Hi I'm a thread!
// Hi I'm a thread!
//
// NOTE: Order is not guaranteed!
// Could also be:
// Hi I'm a thread!
// Hi I'm a thread!
//
// (Both threads print the same message, might interleave)

// EXECUTION TIMELINE:
//
// Time  Main Thread                      Thread1                  Thread2
// ----  -----------                      -------                  -------
// T0    Start main()
// T1    pthread_create(&thread1,...)
// T2    pthread_create returns           (thread1 starts)
// T3    pthread_create(&thread2,...)     printf("Hi...")
// T4    pthread_create returns                                    (thread2 starts)
// T5    pthread_join(thread1) called     return NULL              printf("Hi...")
// T6    (main BLOCKS, waiting...)        (thread1 ends)
// T7    pthread_join returns                                      return NULL
// T8    pthread_join(thread2) called                              (thread2 ends)
// T9    pthread_join returns
// T10   return 0 (program ends)

// CONCURRENCY VISUALIZATION:
//
// Between T2 and T7, TWO threads are running:
// - Main thread (blocked in pthread_join)
// - Thread1 (executing func1)
//
// Between T4 and T10, potentially THREE threads:
// - Main thread (blocked in pthread_join)
// - Thread1 (might still be running)
// - Thread2 (executing func1)

// KEY CONCEPTS:
//
// 1. THREAD vs PROCESS:
//
//    PROCESS (created with fork):
//      - Has its own memory space
//      - Heavy weight (expensive to create)
//      - Communication is complex (pipes, sockets)
//      - Strong isolation (crash doesn't affect others)
//
//    THREAD (created with pthread_create):
//      - Shares memory space with other threads
//      - Light weight (cheap to create)
//      - Communication is easy (shared variables)
//      - Weak isolation (crash kills all threads)
//
// 2. SHARED MEMORY:
//    All threads in a process share:
//      - Global variables
//      - Heap memory (malloc'd memory)
//      - Code (text segment)
//      - Open file descriptors
//
//    Each thread has its own:
//      - Stack (local variables)
//      - Registers (including program counter)
//      - Thread-local storage
//
// 3. FUNCTION POINTER REQUIREMENT:
//    pthread_create REQUIRES a function pointer
//    Because it needs to know WHICH function to execute
//    Different threads can execute different functions
//    func1 is converted to a function pointer automatically
//
// 4. void* AS POLYMORPHISM:
//    void* allows pthread_create to be generic
//    Same function works for all types
//    Alternative would be: pthread_create_int, pthread_create_string, etc.
//    C uses void* to avoid this explosion of functions

// COMPARISON: FORK vs PTHREAD:
//
// PROCESS CREATION (fork):
//   pid_t pid = fork();
//   if (pid == 0) {
//     // child code
//   } else {
//     // parent code
//   }
//
// THREAD CREATION (pthread):
//   pthread_t thread;
//   pthread_create(&thread, NULL, func, NULL);
//   // main continues
//   pthread_join(thread, NULL);
//
// SIMILARITY:
//   Both create concurrent execution
//
// DIFFERENCE:
//   fork: Copies memory (separate spaces)
//   pthread: Shares memory (same space)

// WHY USE THREADS?
//
// 1. PERFORMANCE:
//    Creating threads is faster than creating processes
//    fork() copies entire memory space (expensive)
//    pthread_create() just creates new stack (cheap)
//
// 2. COMMUNICATION:
//    Threads share memory naturally
//    Easy to share data via global variables
//    No need for IPC (pipes, sockets, shared memory)
//
// 3. RESPONSIVENESS:
//    GUI applications: One thread for UI, others for work
//    Web servers: One thread per client request
//    Background tasks while main thread stays responsive
//
// 4. PARALLELISM:
//    On multi-core systems, threads run truly in parallel
//    Can speed up CPU-intensive tasks
//    Example: Render video on 8 cores with 8 threads
//
// 5. RESOURCE SHARING:
//    Threads share file descriptors
//    Opening a file in one thread makes it available in all
//    Useful for shared resources

// WHY TWO THREADS EXECUTE THE SAME FUNCTION?
//
// Threads don't have to execute the same function
// You can do: pthread_create(&t1, NULL, func1, NULL);
//             pthread_create(&t2, NULL, func2, NULL);
//
// In this example, both use func1 because:
//   - Demonstrates multiple threads can share code
//   - Simplicity (don't need to write two functions)
//   - Common pattern (worker pool all execute same worker function)

// PTHREAD_JOIN DETAILS:
//
// MUST JOIN:
//   Every created thread should be joined (or detached)
//   Not joining causes resource leaks
//   Thread's resources aren't freed until joined
//
// CAN ONLY JOIN ONCE:
//   After pthread_join returns, thread is destroyed
//   Joining again is undefined behavior
//
// ORDER DOESN'T MATTER:
//   Can join threads in any order
//   pthread_join(thread2) before pthread_join(thread1) works
//   If thread2 finishes first, pthread_join(thread2) returns immediately
//
// DETACHED THREADS (alternative):
//   pthread_detach(thread) makes thread clean up automatically
//   Don't need to join detached threads
//   Can't get return value from detached threads
//   Useful for fire-and-forget tasks

// COMPILE NOTE:
// You MUST use the -pthread flag when compiling pthread programs
// gcc threads1.c -o threads1 -pthread
//
// WHY -pthread?
// The -pthread flag:
//   - Links the pthread library
//   - Sets necessary compiler flags
//   - Enables thread-safe versions of standard library functions
//   - Defines preprocessor macros for thread safety
//
// WITHOUT -pthread:
//   - Linker error: undefined reference to pthread_create
//   - Even if you manually link -lpthread, might have subtle bugs

// TRY IT:
// 1. Compile: gcc threads1.c -o threads1 -pthread
// 2. Run: ./threads1
// 3. Observe: Two messages printed
//
// EXPERIMENT:
// 1. Comment out pthread_join() lines
//    What happens? (Might not see both messages)
//    Why? (main() exits before threads finish)
//
// 2. Create 10 threads instead of 2
//    Use a loop: for(int i=0; i<10; i++) pthread_create(&threads[i], ...)
//    See 10 messages printed
//
// 3. Add sleep(1) before each pthread_join
//    See that threads finish while main() sleeps
//    (Threads run independently of main)
//
// 4. Try printing pthread_self() inside func1
//    See different thread IDs for each thread
//
// 5. What if you forget to return NULL in func1?
//    Try it! (Undefined behavior, but often returns garbage)
