/*
 * Exercise 1: Race Condition Demonstration
 * Difficulty: 🟢 Beginner
 *
 * OBJECTIVE:
 * Create a program where multiple threads increment a shared counter without
 * synchronization. Observe and measure the "lost updates" problem to understand
 * why synchronization is necessary.
 *
 * REQUIREMENTS:
 * 1. Create a global shared counter initialized to 0
 * 2. Spawn 10 threads, each incrementing the counter 100,000 times
 * 3. Each thread should print a start and completion message
 * 4. The main thread should print the final counter value
 * 5. Calculate and display the number of lost updates
 * 6. Run the program multiple times to observe varying results
 *
 * EXPECTED BEHAVIOR:
 * - Expected final value: 10 × 100,000 = 1,000,000
 * - Actual result: Usually significantly less (varies each run)
 * - This demonstrates the "lost update" problem caused by race conditions
 *
 * SAMPLE OUTPUT (results will vary):
 * Starting race condition demonstration...
 * Expected final value: 1000000
 *
 * Thread 0: Starting increments
 * Thread 1: Starting increments
 * ...
 * Thread 0: Completed 100000 increments
 * Thread 1: Completed 100000 increments
 * ...
 *
 * ===== RESULTS =====
 * Final counter value: 847,392
 * Expected value: 1,000,000
 * Lost updates: 152,608 (15.26%)
 *
 * WHY THIS HAPPENS:
 * The increment operation (counter++) is NOT atomic. It consists of three steps:
 * 1. Read the current value from memory into a register
 * 2. Increment the value in the register
 * 3. Write the value back to memory
 *
 * When multiple threads execute these steps simultaneously, they can interfere
 * with each other, causing some increments to be lost.
 *
 * HINTS:
 * - Declare counter as global: int shared_counter = 0;
 * - Define constants: NUM_THREADS = 10, INCREMENTS_PER_THREAD = 100000
 * - Thread function signature: void* increment_counter(void* arg)
 * - Pass thread ID using: (void*)(long)i
 * - Retrieve in thread: int thread_id = (int)(long)arg;
 * - Use pthread_create() and pthread_join()
 *
 * LEARNING GOALS:
 * - Observe race conditions in action
 * - Understand non-atomic operations
 * - See the lost update problem
 * - Motivate the need for synchronization primitives
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// TODO: Define constants
// #define NUM_THREADS 10
// #define INCREMENTS_PER_THREAD 100000

// TODO: Declare global shared counter
// int shared_counter = 0;

// TODO: Implement thread function
// void* increment_counter(void* arg) {
//     // 1. Get thread ID from argument: int thread_id = (int)(long)arg;
//     // 2. Print start message with thread ID
//     // 3. Loop INCREMENTS_PER_THREAD times:
//     //    - Increment shared_counter (shared_counter++)
//     //    - Optionally add usleep(1) every 10000 iterations to increase race visibility
//     // 4. Print completion message with thread ID
//     // 5. Return NULL
// }

int main() {
    // TODO: Declare thread array
    // pthread_t threads[NUM_THREADS];

    printf("Starting race condition demonstration...\n");
    // TODO: Uncomment after defining constants
    // printf("Expected final value: %d\n\n", NUM_THREADS * INCREMENTS_PER_THREAD);

    // TODO: Create threads
    // for (int i = 0; i < NUM_THREADS; i++) {
    //     int result = pthread_create(&threads[i], NULL, increment_counter, (void*)(long)i);
    //     if (result != 0) {
    //         printf("Error creating thread %d\n", i);
    //         return EXIT_FAILURE;
    //     }
    // }

    // TODO: Wait for all threads to complete
    // for (int i = 0; i < NUM_THREADS; i++) {
    //     pthread_join(threads[i], NULL);
    // }

    // TODO: Calculate results
    // int expected = NUM_THREADS * INCREMENTS_PER_THREAD;
    // int lost = expected - shared_counter;
    // double percent_lost = (lost * 100.0) / expected;

    // TODO: Print results
    // printf("\n===== RESULTS =====\n");
    // printf("Final counter value: %d\n", shared_counter);
    // printf("Expected value: %d\n", expected);
    // printf("Lost updates: %d (%.2f%%)\n", lost, percent_lost);
    // printf("\nThis demonstrates why synchronization is necessary!\n");
    // printf("Exercise 2 will show how to fix this with mutexes.\n");

    return 0;
}
