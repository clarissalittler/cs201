/*
 * Exercise 2: Mutex-Protected Counter
 * Difficulty: 🟢 Beginner
 *
 * OBJECTIVE:
 * Fix the race condition from Exercise 1 by using a mutex to protect the
 * shared counter. Learn proper mutex initialization, locking, and unlocking.
 *
 * REQUIREMENTS:
 * 1. Create the same setup as Exercise 1 (10 threads, 100,000 increments each)
 * 2. Declare a global mutex to protect the counter
 * 3. Initialize the mutex before creating threads
 * 4. Lock the mutex before incrementing, unlock after
 * 5. Verify the final count is always exactly 1,000,000
 * 6. Destroy the mutex after all threads complete
 * 7. Measure and report execution time
 *
 * EXPECTED BEHAVIOR:
 * - Final counter value: ALWAYS exactly 1,000,000
 * - No lost updates
 * - Slower execution than Exercise 1 (due to synchronization overhead)
 *
 * SAMPLE OUTPUT:
 * Starting mutex-protected counter demonstration...
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
 * Final counter value: 1,000,000
 * Expected value: 1,000,000
 * Lost updates: 0 (0.00%)
 * SUCCESS: Counter is correct!
 *
 * CRITICAL SECTION PATTERN:
 * pthread_mutex_lock(&counter_mutex);
 * shared_counter++;  // Only one thread can be here at a time
 * pthread_mutex_unlock(&counter_mutex);
 *
 * HINTS:
 * - Declare mutex: pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
 * - Or use: pthread_mutex_init(&counter_mutex, NULL);
 * - Lock: pthread_mutex_lock(&counter_mutex);
 * - Unlock: pthread_mutex_unlock(&counter_mutex);
 * - Destroy: pthread_mutex_destroy(&counter_mutex);
 * - Keep critical sections small for better performance
 *
 * PERFORMANCE NOTE:
 * This will be slower than Exercise 1 because:
 * - Mutex operations have overhead
 * - Threads must wait for each other
 * - Serialization reduces parallelism
 * BUT it guarantees correctness!
 *
 * LEARNING GOALS:
 * - Use mutexes to protect shared data
 * - Understand critical sections
 * - See the tradeoff between correctness and performance
 * - Learn proper mutex lifecycle (init, lock/unlock, destroy)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// TODO: Define constants
// #define NUM_THREADS 10
// #define INCREMENTS_PER_THREAD 100000

// TODO: Declare global shared counter
// int shared_counter = 0;

// TODO: Declare global mutex
// pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

// TODO: Implement thread function with mutex protection
// void* increment_counter(void* arg) {
//     int thread_id = (int)(long)arg;
//
//     printf("Thread %d: Starting increments\n", thread_id);
//
//     for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
//         // TODO: Lock the mutex
//         // pthread_mutex_lock(&counter_mutex);
//
//         // TODO: Increment the counter (critical section)
//         // shared_counter++;
//
//         // TODO: Unlock the mutex
//         // pthread_mutex_unlock(&counter_mutex);
//     }
//
//     printf("Thread %d: Completed %d increments\n", thread_id, INCREMENTS_PER_THREAD);
//     return NULL;
// }

int main() {
    // TODO: Declare thread array
    // pthread_t threads[NUM_THREADS];

    printf("Starting mutex-protected counter demonstration...\n");
    // TODO: Uncomment after defining constants
    // printf("Expected final value: %d\n\n", NUM_THREADS * INCREMENTS_PER_THREAD);

    // TODO: Record start time
    // struct timespec start, end;
    // clock_gettime(CLOCK_MONOTONIC, &start);

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

    // TODO: Record end time
    // clock_gettime(CLOCK_MONOTONIC, &end);
    // double elapsed = (end.tv_sec - start.tv_sec) +
    //                  (end.tv_nsec - start.tv_nsec) / 1e9;

    // TODO: Calculate results
    // int expected = NUM_THREADS * INCREMENTS_PER_THREAD;
    // int lost = expected - shared_counter;
    // double percent_lost = (lost * 100.0) / expected;

    // TODO: Print results
    // printf("\n===== RESULTS =====\n");
    // printf("Final counter value: %d\n", shared_counter);
    // printf("Expected value: %d\n", expected);
    // printf("Lost updates: %d (%.2f%%)\n", lost, percent_lost);
    // printf("Execution time: %.3f seconds\n", elapsed);
    //
    // if (shared_counter == expected) {
    //     printf("SUCCESS: Counter is correct!\n");
    // } else {
    //     printf("ERROR: Counter is incorrect!\n");
    // }

    // TODO: Destroy the mutex
    // pthread_mutex_destroy(&counter_mutex);

    printf("\nMutexes guarantee correctness at the cost of some performance.\n");

    return 0;
}
