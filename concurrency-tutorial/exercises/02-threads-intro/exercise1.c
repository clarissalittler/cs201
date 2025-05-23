/*
 * Exercise 1: First Thread
 * Difficulty: 🟢 Beginner
 * 
 * OBJECTIVE:
 * Create a program that spawns a single thread. Both main thread and child 
 * thread should print their thread IDs and a message identifying themselves.
 * 
 * REQUIREMENTS:
 * 1. Create one child thread using pthread_create()
 * 2. Both main and child threads should print:
 *    - Their thread ID 
 *    - A message identifying whether they are main or child thread
 * 3. The main thread must wait for the child thread to complete
 * 4. Handle thread creation errors appropriately
 * 
 * EXPECTED OUTPUT (thread IDs will vary):
 * Main thread: Thread ID = 140123456789
 * Child thread: Thread ID = 140123456790
 * Main thread: Child thread has completed
 * 
 * HINTS:
 * - Use pthread_create() to create threads
 * - Use pthread_self() to get current thread ID
 * - Use pthread_join() to wait for thread completion
 * - Thread function signature: void* function_name(void* arg)
 * - Compile with: gcc -o exercise1 exercise1.c -pthread
 * 
 * KEY DIFFERENCES FROM PROCESSES:
 * - Threads share memory space (unlike processes)
 * - Thread IDs are pthread_t type (not integer PIDs)
 * - Use pthread_join() instead of wait()
 * - Much lighter weight than processes
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// TODO: Define the thread function
// Function signature: void* thread_function(void* arg)
// This function should:
// 1. Print the child thread's ID using pthread_self()
// 2. Print a message identifying it as the child thread
// 3. Return NULL

int main() {
    // TODO: Declare a pthread_t variable to store thread ID
    
    // TODO: Print main thread information
    // Use pthread_self() to get main thread ID
    
    // TODO: Create a thread using pthread_create()
    // Arguments: &thread_id, NULL, thread_function, NULL
    // Check return value for errors (0 = success)
    
    // TODO: Wait for the child thread to complete using pthread_join()
    // Arguments: thread_id, NULL
    
    // TODO: Print a completion message from main thread
    
    return 0;
}