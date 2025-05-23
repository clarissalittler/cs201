/*
 * Exercise 4: Shared Counter Problem
 * Difficulty: 🟡 Intermediate
 * 
 * OBJECTIVE:
 * Create multiple threads that increment a shared counter. Observe the race 
 * condition and understand why the final result is unpredictable without 
 * proper synchronization.
 * 
 * REQUIREMENTS:
 * 1. Create 5 threads that each increment a global counter 1000 times
 * 2. Each thread should print its start and completion messages
 * 3. The main thread should print the final counter value
 * 4. Run the program multiple times and observe different results
 * 5. Add timing to make race conditions more visible
 * 
 * EXPECTED BEHAVIOR:
 * - Expected result: 5 × 1000 = 5000
 * - Actual result: Usually less than 5000 (varies each run)
 * - This demonstrates the "lost update" problem
 * 
 * SAMPLE OUTPUT (results will vary):
 * Main thread: Starting 5 counter threads
 * Thread 1: Starting to increment counter
 * Thread 2: Starting to increment counter
 * ...
 * Thread 1: Completed 1000 increments
 * Thread 2: Completed 1000 increments
 * ...
 * Main thread: Final counter value = 4847 (should be 5000)
 * Main thread: Lost updates = 153
 * 
 * HINTS:
 * - Declare counter as global variable: int shared_counter = 0;
 * - Use usleep(1) occasionally to increase chance of race conditions
 * - Each thread should increment in a loop: for(int i = 0; i < 1000; i++)
 * - Run multiple times to see different results
 * 
 * WHY THIS HAPPENS:
 * The increment operation (counter++) is NOT atomic. It consists of:
 * 1. Load counter value from memory
 * 2. Add 1 to the value  
 * 3. Store result back to memory
 * 
 * Multiple threads can interfere with each other during these steps.
 * 
 * LEARNING GOALS:
 * - Understanding race conditions
 * - Observing non-atomic operations
 * - Seeing why synchronization is needed
 * - Introduction to the "lost update" problem
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// TODO: Declare global shared counter variable
// int shared_counter = 0;

// TODO: Define thread function for incrementing counter
// void* increment_counter(void* arg)
// This function should:
// 1. Get thread number from argument
// 2. Print start message
// 3. Loop 1000 times:
//    - Increment shared_counter
//    - Occasionally call usleep(1) to increase race condition chances
// 4. Print completion message
// 5. Return NULL

int main() {
    // TODO: Define constants
    // const int NUM_THREADS = 5;
    // const int INCREMENTS_PER_THREAD = 1000;
    // const int EXPECTED_TOTAL = NUM_THREADS * INCREMENTS_PER_THREAD;
    
    // TODO: Declare thread array
    // pthread_t threads[NUM_THREADS];
    
    printf("Main thread: Starting %d counter threads\n", NUM_THREADS);
    printf("Each thread will increment counter %d times\n", INCREMENTS_PER_THREAD);
    printf("Expected final value: %d\n\n", EXPECTED_TOTAL);
    
    // TODO: Create threads
    // for (int i = 0; i < NUM_THREADS; i++) {
        // TODO: Create thread, pass thread number as argument
        // Use (void*)(long)i to pass the thread number
    // }
    
    // TODO: Wait for all threads
    // for (int i = 0; i < NUM_THREADS; i++) {
        // TODO: Join thread
    // }
    
    // TODO: Print results
    // printf("\nMain thread: Final counter value = %d (expected %d)\n", 
    //        shared_counter, EXPECTED_TOTAL);
    // printf("Lost updates = %d\n", EXPECTED_TOTAL - shared_counter);
    
    // TODO: Print explanation
    // printf("\nWhy this happens:\n");
    // printf("- The increment operation is NOT atomic\n");
    // printf("- Multiple threads interfere with each other\n");
    // printf("- This is called a 'race condition'\n");
    // printf("- Next exercises will show how to fix this\n");
    
    return 0;
}