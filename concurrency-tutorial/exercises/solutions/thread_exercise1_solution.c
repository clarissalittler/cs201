/*
 * SOLUTION: Thread Exercise 1 - First Thread
 * 
 * This solution demonstrates basic thread creation and management using
 * the pthread library. It shows the fundamental difference between
 * processes and threads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Thread function - must have this signature
void* child_thread_function(void* arg) {
    (void)arg; // Suppress unused parameter warning
    // Print child thread information
    printf("Child thread: Thread ID = %lu\n", pthread_self());
    
    // Simulate some work
    usleep(100000); // 100ms
    
    printf("Child thread: Finishing work\n");
    
    // Return NULL (threads return void*)
    return NULL;
}

int main() {
    pthread_t child_thread;
    int result;
    
    // Print main thread information
    printf("Main thread: Thread ID = %lu\n", pthread_self());
    printf("Main thread: Creating child thread\n");
    
    // Create a child thread
    result = pthread_create(&child_thread, NULL, child_thread_function, NULL);
    
    if (result != 0) {
        printf("Error: Failed to create thread (error code: %d)\n", result);
        return EXIT_FAILURE;
    }
    
    printf("Main thread: Child thread created successfully\n");
    
    // Wait for the child thread to complete
    result = pthread_join(child_thread, NULL);
    
    if (result != 0) {
        printf("Error: Failed to join thread (error code: %d)\n", result);
        return EXIT_FAILURE;
    }
    
    printf("Main thread: Child thread has completed\n");
    printf("Main thread: Program finished\n");
    
    return 0;
}

/*
 * EXPLANATION:
 * 
 * 1. THREAD CREATION:
 *    - pthread_create() creates a new thread
 *    - Arguments: thread_id, attributes, function, argument
 *    - Returns 0 on success, error code on failure
 * 
 * 2. THREAD FUNCTION:
 *    - Must have signature: void* function_name(void* arg)
 *    - Receives argument as void* (can be anything)
 *    - Returns void* (can return any pointer)
 * 
 * 3. THREAD JOINING:
 *    - pthread_join() waits for thread to complete
 *    - Similar to wait() for processes
 *    - Prevents zombie threads
 * 
 * 4. THREAD IDs:
 *    - pthread_self() returns current thread ID
 *    - Thread IDs are pthread_t type (opaque)
 *    - Cast to unsigned long for printing
 * 
 * 5. ERROR HANDLING:
 *    - pthread functions return 0 on success
 *    - Non-zero return indicates error
 *    - Always check return values!
 * 
 * KEY DIFFERENCES FROM PROCESSES:
 * 
 * | Aspect | Processes | Threads |
 * |--------|-----------|---------|
 * | Creation | fork() | pthread_create() |
 * | ID Type | pid_t (int) | pthread_t (opaque) |
 * | Memory | Separate | Shared |
 * | Variables | Copied | Shared |
 * | Waiting | wait() | pthread_join() |
 * | Overhead | High | Low |
 * 
 * MEMORY SHARING:
 * Unlike processes, threads share:
 * - Global variables
 * - Heap memory
 * - Open file descriptors
 * - Signal handlers
 * 
 * Each thread has its own:
 * - Stack
 * - Registers
 * - Program counter
 * 
 * COMMON MISTAKES:
 * - Forgetting to link with -pthread
 * - Not joining threads (creates resource leaks)
 * - Incorrect function signature for thread function
 * - Not checking return values for errors
 */