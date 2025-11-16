// PEDAGOGICAL PURPOSE:
// This program introduces POSIX threads (pthreads) - a lighter-weight alternative
// to process-based concurrency. This is the fundamental "Hello World" of threading.
// Key learning objectives:
// 1. Understanding what a thread is (lightweight process, shared memory)
// 2. Creating threads with pthread_create()
// 3. Waiting for thread completion with pthread_join()
// 4. Understanding thread functions and their signatures
// 5. Difference between threads and processes (fork vs pthread)
//
// KEY CONCEPTS:
// - Threads share the same memory space (unlike processes)
// - Much cheaper to create than processes (no memory copying)
// - Need to synchronize access to shared data (covered in thread2.c)
// - Thread lifecycle: create -> execute -> join -> cleanup

#include <stdio.h>
#include <pthread.h>    // POSIX thread library
                        // Compile with: gcc -pthread thread1.c -o thread1
                        // WHY -pthread? Links the pthread library

// THREAD FUNCTION:
// This function will execute in the NEW thread (not the main thread)
// Thread functions MUST have this exact signature:
//   void* function_name(void* argument)
//
// WHY void*?
// - void* is a generic pointer (can point to anything)
// - Allows passing ANY type of data to the thread
// - Return void* allows returning ANY type of data from the thread
// - This is C's way of being "generic" (no templates like C++)
void* thread_function(void* arg) {
    // THIS CODE RUNS IN THE NEW THREAD:
    // The new thread executes this function concurrently with main()
    //
    // IMPORTANT: This is NOT main()!
    // The main thread continues executing main() while this runs
    // Both threads execute AT THE SAME TIME (concurrently)

    printf("Thread executing\n");
    // This prints from the new thread
    // It might print BEFORE or AFTER main thread's printf - we don't know!

    return NULL;
    // Return NULL (no meaningful return value in this simple example)
    // More complex examples might return a pointer to results
    //
    // After return, the thread TERMINATES
    // Its resources aren't fully cleaned up until pthread_join()
}

int main() {
    // THREAD HANDLE:
    // pthread_t is an opaque type representing a thread
    // Think of it as a "thread ID" or "thread handle"
    // We'll use this to refer to our thread later (for joining)
    pthread_t thread;

    // UNUSED VARIABLE (from original code):
    // This variable is declared but never used - probably left over
    // from a more complex example where data would be passed to thread
    char* message = "Hello from main thread";
    // In a real program, we might pass this to thread_function via arg

    // CREATE THE THREAD:
    // pthread_create() is the key function for creating threads
    // It's analogous to fork() but creates a thread instead of a process
    //
    // SIGNATURE: int pthread_create(pthread_t *thread,
    //                                const pthread_attr_t *attr,
    //                                void *(*start_routine)(void*),
    //                                void *arg);
    //
    // ARGUMENTS EXPLAINED:
    // 1. &thread: Address where thread ID will be stored
    //    WHY &? pthread_create needs to WRITE the thread ID
    //
    // 2. NULL: Thread attributes (default = NULL)
    //    Could specify stack size, scheduling policy, etc.
    //    NULL means "use default attributes"
    //
    // 3. thread_function: Function that the new thread will execute
    //    This is a FUNCTION POINTER
    //    The new thread starts executing at the beginning of this function
    //
    // 4. NULL: Argument to pass to thread_function
    //    This would be the void* arg parameter in thread_function
    //    NULL means "no argument"
    //    Could pass &message to send data to the thread
    pthread_create(&thread, NULL, thread_function, NULL);

    // WHAT JUST HAPPENED?
    // - A new thread was created
    // - The new thread starts executing thread_function()
    // - The main thread continues HERE immediately (doesn't wait)
    // - Now TWO threads are running concurrently:
    //   1. Main thread (executing main())
    //   2. New thread (executing thread_function())
    //
    // CRITICAL DIFFERENCE FROM fork():
    // - fork() creates a new PROCESS (separate memory)
    // - pthread_create() creates a new THREAD (shared memory)
    // - Both threads can access ALL variables in this program!
    // - This is faster but requires synchronization (see thread2.c)

    // WAIT FOR THREAD TO COMPLETE:
    // pthread_join() waits for the specified thread to terminate
    // Similar to wait() for processes, but for threads
    //
    // SIGNATURE: int pthread_join(pthread_t thread, void **retval);
    //
    // ARGUMENTS:
    // 1. thread: Which thread to wait for (from pthread_create)
    // 2. NULL: Where to store the thread's return value
    //    If not NULL, would receive the value returned by thread_function
    //    NULL means "we don't care about the return value"
    pthread_join(thread, NULL);

    // WHY JOIN?
    // 1. Ensure thread completes before main() exits
    //    If main() exits, ALL threads are killed (program terminates)
    //    pthread_join() prevents this premature termination
    //
    // 2. Clean up thread resources
    //    After join, thread's resources are fully reclaimed
    //    Without join, thread becomes "detached" (resources leak)
    //
    // 3. Synchronization
    //    Main thread WAITS here until new thread finishes
    //    This ensures "Thread executing" prints before "Thread joined"
    //
    // BLOCKING BEHAVIOR:
    // pthread_join() BLOCKS (waits) until the thread terminates
    // Main thread STOPS here until thread_function() returns
    // This creates a synchronization point

    printf("Thread joined\n");
    // This prints AFTER the thread completes
    // We're guaranteed that "Thread executing" printed first
    // (because pthread_join waits for the thread to finish)

    return 0;
    // Main thread exits
    // Since we joined the thread, we know it's already done
    // Program terminates cleanly
}

// EXECUTION TIMELINE:
//
// Time    Main Thread                          New Thread
// ----    -----------                          ----------
// T0      Start main()
// T1      Declare variables
// T2      Call pthread_create()  ------------> Thread created
// T3      Continue immediately                 Start thread_function()
// T4      Call pthread_join()                  printf("Thread executing\n")
// T5      BLOCK (waiting)                      return NULL
// T6      Unblock (thread done) <------------ Thread terminates
// T7      printf("Thread joined\n")
// T8      return 0
// T9      Program exits

// EXPECTED OUTPUT:
// Thread executing
// Thread joined
//
// The order is DETERMINISTIC here because pthread_join() waits!
// "Thread joined" MUST come after "Thread executing"

// MEMORY SHARING:
//
// CRITICAL DIFFERENCE FROM PROCESSES:
// Both threads share the SAME memory space
//
// SHARED:                          NOT SHARED:
// - Global variables               - Stack (each thread has its own)
// - Heap allocations              - Local variables (on separate stacks)
// - Code segment                  - Thread ID
// - Open file descriptors         - Register state
// - Static variables              - Return address
//
// Example: If we had a global variable:
//   int counter = 0;
//
// BOTH threads could access and modify 'counter'
// This is powerful but dangerous (race conditions - see thread2.c)

// PROCESS vs THREAD COMPARISON:
//
// PROCESSES (fork):                 THREADS (pthread_create):
// -------------------------         ---------------------------
// Separate memory spaces            Shared memory space
// Expensive to create               Cheap to create
// Isolated (can't corrupt)          Can corrupt each other
// Need IPC to communicate           Direct memory sharing
// More robust                       More efficient
// Security isolation                No isolation
// ~10,000s to create                ~10s of microseconds to create
// MB of memory overhead             KB of memory overhead
//
// WHEN TO USE EACH?
// Processes: When you need isolation, security, or robustness
// Threads: When you need speed, shared data, or many concurrent tasks

// PTHREAD COMPILATION:
//
// IMPORTANT: Must compile with -pthread flag
//   gcc thread1.c -o thread1 -pthread
//   OR
//   gcc -pthread thread1.c -o thread1
//
// WHY?
// - Links the pthread library (libpthread)
// - Enables thread-safe compilation
// - Without it: "undefined reference to pthread_create" error

// THREAD FUNCTION SIGNATURE DETAILS:
//
// void* thread_function(void* arg)
//
// WHY this specific signature?
// - POSIX standard requires it for pthread_create
// - void* argument: Can pass any type of data
//   Example: pthread_create(&thread, NULL, thread_function, &message);
//   Then in thread_function:
//     char* msg = (char*)arg;
//     printf("%s\n", msg);
//
// - void* return: Can return any type of data
//   Example: return (void*)42;
//   Then in main:
//     void* result;
//     pthread_join(thread, &result);
//     int value = (int)result;

// COMMON MISTAKES:
//
// 1. Forgetting pthread_join():
//    main() exits before thread completes -> thread killed prematurely
//
// 2. Forgetting -pthread flag:
//    Compilation error: "undefined reference to pthread_create"
//
// 3. Wrong function signature:
//    int thread_function() { ... }  // WRONG!
//    void* thread_function(void* arg) { ... }  // CORRECT
//
// 4. Passing stack addresses:
//    int x = 5;
//    pthread_create(&thread, NULL, thread_function, &x);  // DANGEROUS!
//    // If main's stack frame changes, &x might be invalid
//    // Better to pass heap-allocated data or static data

// ADVANTAGES OF THREADS OVER PROCESSES:
//
// 1. SPEED: Creating threads is ~100x faster than fork()
//    Example: 10 microseconds vs 1 millisecond
//
// 2. MEMORY: Threads share memory, no copying needed
//    Example: 100 threads might use 110MB, 100 processes might use 1GB
//
// 3. COMMUNICATION: Direct memory access, no pipes/sockets needed
//    Example: Both threads can access 'message' variable
//
// 4. CONTEXT SWITCHING: Switching threads is faster than switching processes
//    Example: Thread switch ~1 microsecond, process switch ~10 microseconds

// DISADVANTAGES OF THREADS:
//
// 1. COMPLEXITY: Need synchronization (locks, mutexes)
//    See thread2.c for race condition example
//
// 2. BUGS: Race conditions, deadlocks, data corruption
//    One thread's bug can crash entire program
//
// 3. NO ISOLATION: All threads share memory
//    Can't run untrusted code safely
//
// 4. DEBUGGING: Non-deterministic bugs are hard to reproduce

// WHAT'S NEXT?
//
// thread2.c demonstrates the DANGER of shared memory:
// - Multiple threads modifying the same variable
// - Race conditions causing incorrect results
// - Why we need mutexes/locks for synchronization
// - The classic "lost update" problem

// TRY IT:
// Compile: gcc -pthread thread1.c -o thread1
// Run: ./thread1
//
// EXPERIMENTS:
// 1. Remove pthread_join() - what happens?
// 2. Add sleep(1) in thread_function - see main wait
// 3. Create multiple threads - need multiple pthread_create/join calls
// 4. Pass &message as 4th argument to pthread_create, use it in thread_function
// 5. Return a value from thread_function, retrieve it with pthread_join

// FURTHER READING:
// - man pthread_create
// - man pthread_join
// - POSIX threads (pthreads) tutorial
// - "Programming with POSIX Threads" by David Butenhof
