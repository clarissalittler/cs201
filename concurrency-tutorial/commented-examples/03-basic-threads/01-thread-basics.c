/**
 * ============================================================================
 * Thread Basics - Creating and Joining Threads
 * ============================================================================
 * 
 * This example demonstrates the fundamentals of working with POSIX threads
 * (pthreads). We'll cover:
 * - Creating threads
 * - Passing arguments to threads
 * - Joining threads (waiting for them to finish)
 * - Returning values from threads
 * 
 * Threads are "lightweight processes" that share the same memory space
 * but can execute independently.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>  /* POSIX threads library */
#include <unistd.h>   /* For sleep() */

/* 
 * Thread function prototype
 * pthread functions require this specific signature:
 * - Return type must be void*
 * - Single parameter must be void*
 */
void* thread_function(void* arg);

/* Another thread function that returns a value */
void* compute_thread(void* arg);

int main() {
    /* 
     * Thread identifiers
     * pthread_t is an opaque type that stores thread info
     */
    pthread_t thread1, thread2, thread3;
    
    /* Variables to store thread return values */
    void* result;
    int* computed_value;
    
    printf("Main thread (ID: %lu) starting...\n", 
           (unsigned long)pthread_self());
    
    /* ================== CREATING THREADS ================== */
    
    /* 
     * 1. Create a simple thread with no arguments
     * pthread_create parameters:
     * - Pointer to thread identifier (output)
     * - Thread attributes (NULL for defaults)
     * - Function to execute
     * - Argument to pass to the function (NULL for none)
     */
    printf("\nCreating thread 1 (no arguments)...\n");
    if (pthread_create(&thread1, NULL, thread_function, NULL) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    
    /* 
     * 2. Create a thread passing a string argument
     * We're passing a literal string as the argument
     */
    printf("Creating thread 2 (with string argument)...\n");
    if (pthread_create(&thread2, NULL, thread_function, "Hello from main thread!") != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    
    /* 
     * 3. Create a thread that computes and returns a value
     * We'll pass an integer argument (allocated on the heap)
     */
    printf("Creating thread 3 (compute thread)...\n");
    int* number = malloc(sizeof(int));  /* Must be heap-allocated for thread safety */
    *number = 5;
    
    if (pthread_create(&thread3, NULL, compute_thread, number) != 0) {
        perror("pthread_create");
        free(number);
        return EXIT_FAILURE;
    }
    
    /* 
     * At this point, all three threads are running concurrently
     * with the main thread. The order of execution is determined
     * by the thread scheduler.
     */
    printf("\nMain thread: All threads created. Main thread continues...\n");
    
    /* Main thread can do other work here while threads run */
    sleep(1);
    
    /* ================== JOINING THREADS ================== */
    
    /* 
     * Wait for thread1 to finish
     * pthread_join parameters:
     * - Thread identifier to wait for
     * - Pointer to store the thread's return value (NULL to ignore)
     */
    printf("\nMain thread: Waiting for thread 1 to finish...\n");
    if (pthread_join(thread1, NULL) != 0) {
        perror("pthread_join thread1");
        return EXIT_FAILURE;
    }
    printf("Main thread: Thread 1 has terminated.\n");
    
    /* Wait for thread2 to finish, and get its return value */
    printf("Main thread: Waiting for thread 2 to finish...\n");
    if (pthread_join(thread2, &result) != 0) {
        perror("pthread_join thread2");
        return EXIT_FAILURE;
    }
    printf("Main thread: Thread 2 has terminated and returned: %s\n", 
           (char*)result);
    
    /* Wait for thread3 to finish, and get its computed value */
    printf("Main thread: Waiting for thread 3 to finish...\n");
    if (pthread_join(thread3, (void**)&computed_value) != 0) {
        perror("pthread_join thread3");
        return EXIT_FAILURE;
    }
    
    printf("Main thread: Thread 3 has terminated and computed: %d\n", 
           *computed_value);
    
    /* Clean up the allocated memory */
    free(computed_value);  /* Thread 3 allocated this */
    /* Note: 'number' was freed by thread3 */
    
    printf("\nMain thread: All threads have completed. Exiting.\n");
    
    return EXIT_SUCCESS;
}

/**
 * Simple thread function that demonstrates thread basics.
 * 
 * @param arg Pointer to the argument passed to the thread
 * @return A string message
 */
void* thread_function(void* arg) {
    /* Get current thread ID */
    pthread_t tid = pthread_self();
    
    printf("Thread %lu: Starting execution...\n", (unsigned long)tid);
    
    /* Check if we received an argument */
    if (arg != NULL) {
        printf("Thread %lu: Received argument: %s\n", 
               (unsigned long)tid, (char*)arg);
    } else {
        printf("Thread %lu: No argument received\n", (unsigned long)tid);
    }
    
    /* Simulate some work */
    printf("Thread %lu: Working...\n", (unsigned long)tid);
    sleep(2);
    printf("Thread %lu: Work completed!\n", (unsigned long)tid);
    
    /* 
     * Return a value to the joining thread
     * For thread2, we'll return a static string.
     * NOTE: Never return a pointer to a local variable!
     */
    return "Thread completed successfully";
}

/**
 * Thread function that computes a value and returns it.
 * Demonstrates allocation and returning of a result.
 * 
 * @param arg Pointer to an integer input value
 * @return Pointer to the computed result (factorial of input)
 */
void* compute_thread(void* arg) {
    pthread_t tid = pthread_self();
    int input = *((int*)arg);
    
    printf("Compute thread %lu: Starting computation with input %d...\n", 
           (unsigned long)tid, input);
    
    /* Calculate factorial of input */
    int result = 1;
    for (int i = 1; i <= input; i++) {
        result *= i;
    }
    
    /* 
     * Allocate memory for the result
     * IMPORTANT: This memory must be freed by the joining thread!
     */
    int* result_ptr = malloc(sizeof(int));
    *result_ptr = result;
    
    printf("Compute thread %lu: Computed factorial(%d) = %d\n", 
           (unsigned long)tid, input, result);
    
    /* Free the input argument (it was allocated in main) */
    free(arg);
    
    /* Return the result */
    return result_ptr;
}

/**
 * EXPLANATION:
 * 
 * POSIX Threads (pthreads) Basics:
 * 
 * 1. Thread Concept:
 *    - Threads are lightweight units of execution within a process
 *    - All threads in a process share the same memory space, file descriptors, etc.
 *    - Each thread has its own stack, registers, and thread ID
 *    - Threads can run concurrently, potentially on different CPU cores
 * 
 * 2. Key pthread Functions:
 *    - pthread_create(): Create a new thread
 *    - pthread_join(): Wait for a thread to terminate
 *    - pthread_self(): Get the calling thread's ID
 *    - pthread_exit(): Terminate the calling thread
 *    - pthread_detach(): Mark a thread as detached (resources auto-reclaimed)
 * 
 * 3. Thread Creation:
 *    - pthread_create() starts a new thread running the specified function
 *    - The function must have the signature: void* func(void* arg)
 *    - A single argument can be passed to the thread function
 *    - For multiple arguments, use a structure and pass its pointer
 * 
 * 4. Thread Joining:
 *    - pthread_join() waits for a thread to terminate
 *    - It retrieves the thread's return value (from pthread_exit or return)
 *    - Joining is necessary to avoid memory leaks (thread resources)
 *    - Alternative is pthread_detach() for "fire and forget" threads
 * 
 * 5. Passing/Returning Data:
 *    - Arguments must be cast to/from void* (C-style type casting)
 *    - For simple types, be careful about passing stack-allocated variables
 *    - For complex data or return values, usually use heap allocation (malloc)
 *    - Always ensure proper memory management across thread boundaries
 * 
 * 6. Thread Safety Considerations:
 *    - Avoid returning pointers to stack variables (they become invalid)
 *    - Be careful with shared data access (potential race conditions)
 *    - For thread safety, use synchronization mechanisms (mutexes, etc.)
 *    - Ensure proper memory management and ownership
 * 
 * 7. Compiling:
 *    - Link with pthread library: gcc -pthread thread_example.c -o thread_example
 */