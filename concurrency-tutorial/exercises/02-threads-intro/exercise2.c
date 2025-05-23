/*
 * Exercise 2: Thread Arguments
 * Difficulty: 🟢 Beginner
 * 
 * OBJECTIVE:
 * Write a program that creates 3 threads, each receiving a different integer 
 * argument. Each thread should print its argument and thread ID.
 * 
 * REQUIREMENTS:
 * 1. Create exactly 3 threads
 * 2. Pass different integer values to each thread (e.g., 10, 20, 30)
 * 3. Each thread should print:
 *    - Its thread ID
 *    - The argument value it received
 *    - A message with its "worker number"
 * 4. Main thread should wait for all threads to complete
 * 5. Handle thread creation errors
 * 
 * EXPECTED OUTPUT (thread IDs and order may vary):
 * Main thread: Creating 3 worker threads
 * Worker 1: Thread ID = 140123456790, Argument = 10
 * Worker 2: Thread ID = 140123456791, Argument = 20  
 * Worker 3: Thread ID = 140123456792, Argument = 30
 * Main thread: All workers completed
 * 
 * HINTS:
 * - To pass an integer to a thread, cast it to void*: (void*)(long)value
 * - In the thread function, cast back: int arg = (long)argument;
 * - Use an array to store thread IDs: pthread_t threads[3];
 * - Use a loop to create and join threads
 * 
 * LEARNING GOALS:
 * - Passing arguments to threads
 * - Managing multiple threads
 * - Understanding void* parameter passing
 * - Thread lifecycle management
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// TODO: Define thread function that accepts void* argument
// This function should:
// 1. Cast the void* argument back to integer
// 2. Print thread ID using pthread_self()
// 3. Print the argument value
// 4. Print worker identification message
// 5. Return NULL

int main() {
    // TODO: Define an array to store thread IDs
    // pthread_t threads[3];
    
    // TODO: Define arguments to pass to each thread
    // int args[] = {10, 20, 30};
    
    printf("Main thread: Creating 3 worker threads\n");
    
    // TODO: Create 3 threads using a loop
    // for (int i = 0; i < 3; i++) {
        // TODO: Create thread with pthread_create()
        // Pass args[i] as argument: (void*)(long)args[i]
        // Store thread ID in threads[i]
        // Check for errors
    // }
    
    // TODO: Wait for all threads to complete
    // for (int i = 0; i < 3; i++) {
        // TODO: Join each thread using pthread_join()
    // }
    
    printf("Main thread: All workers completed\n");
    return 0;
}