// PEDAGOGICAL PURPOSE:
// This program introduces POSIX threads (pthreads) - the basics of creating and joining threads.
// Key learning objectives:
// 1. Understanding what a thread is (lightweight process sharing memory)
// 2. Learning the pthread_create() function for creating threads
// 3. Understanding pthread_join() for waiting for thread completion
// 4. Learning the required function signature: void* (*)(void*)
// 5. Understanding the difference between void and void*
// 6. Seeing why function pointers are necessary for thread creation
// 7. Understanding the pthread_t type for thread handles
// 8. Introducing the concept of concurrent execution

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

// IMPORTANT COMMENT FROM ORIGINAL CODE:
// This explains a CRITICAL concept in C that's often confusing
/*
  void and void* mean extremely different things

  void is the type of nothing
    - Used for functions that don't return a value
    - Used for functions that take no parameters
    - Examples: void foo(void), void bar()

  void* is the universal donor of pointer types
    - A pointer to "something" but we don't know what
    - ANY pointer type can be converted to void*
    - void* can be converted to any pointer type (with a cast)
    - This is C's way of achieving generic/polymorphic pointers
    - Examples: void* ptr can hold int*, char*, struct foo*, etc.

  (casting with pointers is *weird*)
    - When you cast pointers, you're not converting data
    - You're reinterpreting the same memory address as a different type
    - This can be dangerous if done incorrectly!
*/

// WHY void* FOR THREADS?
// void* is the only way to get polymorphism out of C
// the pthread_create function
// has to take the most generic possible
// function type as an argument
// and that's a thing that takes a void*
// and returns a void*
//
// THINK ABOUT IT:
// pthread_create needs to work with ANY kind of thread function
// - A thread that processes integers
// - A thread that processes strings
// - A thread that processes custom structs
// - A thread that takes no data
//
// The solution: void*!
// Every thread function has the SAME signature
// But inside, we can cast void* to whatever we actually need

// THREAD FUNCTION:
// This is the function that the thread will execute
//
// REQUIRED SIGNATURE:
// void* func(void* arg)
//   - MUST return void* (even if we don't return anything useful)
//   - MUST take void* as parameter (even if we don't use it)
//
// This signature is REQUIRED by pthread_create
// If you try a different signature, you'll get a compiler error
//
// WHAT THIS FUNCTION DOES:
// Just prints a message to show the thread is running
void* func(void* arg){

  // SIMPLE OUTPUT:
  // Print from the thread to show it's executing
  // When you run this, you'll see this message
  printf("Hi I'm a thread!\n");

  // RETURN NULL:
  // We must return void* (even though we don't have useful data)
  // NULL is a valid void* value meaning "no data"
  //
  // In later examples, we'll return actual data via this mechanism
  return NULL;
}

int main(){

  // THREAD HANDLE:
  // pthread_t is an opaque type (we don't know what's inside)
  // It's a HANDLE - a reference to a thread
  // The OS uses this to identify and manage the thread
  //
  // ANALOGY:
  // Think of pthread_t like a ticket stub
  // You give it to pthread_join to "claim" the thread's result
  pthread_t thread; // the data structure that holds all the information about a thread

  // CREATE THE THREAD:
  // pthread_create is the system call to create a new thread
  //
  // FUNCTION SIGNATURE:
  // int pthread_create(
  //   pthread_t *thread,              // Where to store the thread handle
  //   const pthread_attr_t *attr,     // Thread attributes (NULL = default)
  //   void *(*start_routine)(void*),  // Function to run (FUNCTION POINTER!)
  //   void *arg                       // Argument to pass to the function
  // )
  //
  // PARAMETERS EXPLAINED:
  // &thread: Address where pthread_create will store the thread ID
  // NULL: Use default attributes (stack size, scheduling, etc.)
  // func: The function for the thread to execute (NOTE: function pointer!)
  // NULL: We're not passing any argument to func (so arg will be NULL)
  //
  // FUNCTION POINTER MAGIC:
  // The third argument is a FUNCTION POINTER!
  // We're passing the ADDRESS of the function 'func'
  // This is WHY we learned about function pointers!
  //
  // WHAT HAPPENS:
  // 1. A new thread is created
  // 2. The new thread starts executing func(NULL)
  // 3. The main thread continues immediately (doesn't wait)
  // 4. Both threads now run CONCURRENTLY
  pthread_create(&thread,NULL,func,NULL);

  // COMMENT FROM ORIGINAL CODE:
  // the third argument of pthread_create
  // is a function!
  // hence why I was talking about function pointers
  // it's not (entirely) because I'm whacked out with poo-brain

  // WAIT FOR THREAD TO COMPLETE:
  // pthread_join blocks until the specified thread terminates
  //
  // FUNCTION SIGNATURE:
  // int pthread_join(
  //   pthread_t thread,    // The thread to wait for
  //   void **retval        // Where to store the thread's return value
  // )
  //
  // PARAMETERS EXPLAINED:
  // thread: The thread handle from pthread_create
  // NULL: We don't care about the return value (func returns NULL anyway)
  //
  // ANALOGY:
  // This is like wait() for processes
  // Parent process calls wait() to wait for child process
  // Main thread calls pthread_join() to wait for spawned thread
  //
  // WHY NECESSARY?
  // Without pthread_join:
  //   - main might exit before the thread finishes
  //   - When main exits, ALL threads are terminated
  //   - The thread might not get a chance to print!
  //
  // With pthread_join:
  //   - main waits until thread completes
  //   - Ensures thread has finished its work before program exits
  pthread_join(thread,NULL);


  return 0;
}

// EXPECTED OUTPUT:
// Hi I'm a thread!

// EXECUTION FLOW:
//
// Time  Main Thread                          Created Thread
// ----  -----------                          --------------
// T0    Start main()
// T1    pthread_create() called
// T2    pthread_create() returns             (thread starts)
// T3    pthread_join() called (BLOCKS)       printf("Hi I'm a thread!\n")
// T4    (waiting...)                         return NULL (thread ends)
// T5    pthread_join() returns
// T6    return 0 (program ends)

// CONCURRENCY:
// Between T2 and T4, TWO threads are running concurrently:
// 1. Main thread (blocked in pthread_join)
// 2. Created thread (executing func)

// KEY CONCEPTS:
//
// 1. THREAD vs PROCESS:
//    PROCESS:
//      - Has its own memory space
//      - Heavy weight (expensive to create)
//      - Communication is complex (pipes, sockets, shared memory)
//      - Created with fork()
//
//    THREAD:
//      - Shares memory space with other threads in same process
//      - Light weight (cheap to create)
//      - Communication is easy (shared variables)
//      - Created with pthread_create()
//
// 2. SHARED MEMORY:
//    All threads in a process share:
//      - Global variables
//      - Heap memory (malloc'd memory)
//      - Code (text segment)
//
//    Each thread has its own:
//      - Stack (local variables)
//      - Registers
//      - Program counter
//
// 3. FUNCTION POINTER REQUIREMENT:
//    pthread_create NEEDS a function pointer
//    Because it needs to know WHICH function the new thread should execute
//    Different threads can execute different functions

// PTHREAD vs FORK:
//
// FORK (processes):
//   pid_t pid = fork();
//   if (pid == 0) {
//     // child code
//   }
//
// PTHREAD (threads):
//   pthread_t thread;
//   pthread_create(&thread, NULL, func, NULL);
//   pthread_join(thread, NULL);
//
// SIMILARITY:
//   Both create concurrent execution
//
// DIFFERENCE:
//   fork: Separate memory spaces
//   pthread: Shared memory space

// WHY USE THREADS?
//
// 1. PERFORMANCE:
//    Threads are lighter weight than processes
//    Creating/destroying threads is faster
//
// 2. SHARED MEMORY:
//    Easy to share data between threads
//    No need for IPC (inter-process communication)
//
// 3. RESPONSIVENESS:
//    GUI applications: One thread for UI, others for work
//    Web servers: One thread per request
//
// 4. PARALLELISM:
//    On multi-core systems, threads run truly in parallel
//    Can speed up CPU-intensive tasks

// COMPILE NOTE:
// You MUST use the -pthread flag when compiling pthread programs
// gcc threads1.c -o threads1 -pthread
//
// WHY?
// The -pthread flag:
//   - Links the pthread library
//   - Sets necessary compiler flags
//   - Enables thread-safe versions of standard library functions

// TRY IT:
// 1. Compile: gcc threads1.c -o threads1 -pthread
// 2. Run: ./threads1
// 3. EXPERIMENT: Comment out pthread_join() - what happens?
//    (The program might exit before the thread prints)
// 4. EXPERIMENT: Create multiple threads in a loop
//    for(int i=0; i<5; i++) pthread_create(...);
//    What output do you see?
// 5. EXPERIMENT: Add sleep(1); before pthread_join()
//    Does the thread still run? (Yes! They run concurrently)
// 6. CHALLENGE: What happens if you join a thread twice?
//    (Undefined behavior - don't do it!)
