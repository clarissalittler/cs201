/**
 * ============================================================================
 * Mutex Basics - Protecting Shared Resources
 * ============================================================================
 * 
 * This example demonstrates the fundamental concepts of mutual exclusion
 * (mutex) synchronization in multi-threaded programs. We'll cover:
 * - What race conditions are and why they occur
 * - How to use mutexes to protect shared data
 * - Mutex initialization, locking, and unlocking
 * - The concept of critical sections
 * 
 * Mutexes are the most basic and essential synchronization primitive for
 * multi-threaded programming.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* Number of increment operations each thread will perform */
#define ITERATIONS 100000

/* Number of threads we'll create */
#define NUM_THREADS 4

/* 
 * Global shared counter variable
 * This will be accessed by multiple threads simultaneously
 */
long shared_counter = 0;

/* 
 * Mutex for protecting access to the shared counter
 * This ensures only one thread can modify the counter at a time
 */
pthread_mutex_t counter_mutex;

/* Thread function prototypes */
void* increment_without_mutex(void* arg);
void* increment_with_mutex(void* arg);

int main() {
    pthread_t threads[NUM_THREADS];
    int i, rc;
    
    printf("Mutex demonstration: protecting shared data from race conditions\n\n");
    
    /* ================== PART 1: WITHOUT MUTEX (INCORRECT) ================== */
    
    printf("Part 1: Demonstrating race conditions (WITHOUT mutex protection)\n");
    printf("Each of %d threads will increment a counter %d times\n", 
           NUM_THREADS, ITERATIONS);
    printf("Expected final counter value: %d\n", NUM_THREADS * ITERATIONS);
    
    /* Reset the shared counter */
    shared_counter = 0;
    
    /* Create threads that will increment without proper synchronization */
    for (i = 0; i < NUM_THREADS; i++) {
        rc = pthread_create(&threads[i], NULL, increment_without_mutex, NULL);
        if (rc != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }
    
    /* Wait for all threads to complete */
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    /* 
     * Print the final counter value
     * This will likely be LESS than the expected value due to race conditions
     */
    printf("Final counter value WITHOUT mutex: %ld\n", shared_counter);
    printf("This value is %s the expected value\n", 
           (shared_counter == NUM_THREADS * ITERATIONS) ? "equal to" : "LESS THAN");
    printf("\n");
    
    /* ================== PART 2: WITH MUTEX (CORRECT) ================== */
    
    printf("Part 2: Using a mutex to prevent race conditions\n");
    
    /* Reset the shared counter */
    shared_counter = 0;
    
    /* Initialize the mutex before using it */
    if (pthread_mutex_init(&counter_mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        return EXIT_FAILURE;
    }
    
    /* Create threads that will increment with proper synchronization */
    for (i = 0; i < NUM_THREADS; i++) {
        rc = pthread_create(&threads[i], NULL, increment_with_mutex, NULL);
        if (rc != 0) {
            perror("pthread_create");
            pthread_mutex_destroy(&counter_mutex);
            return EXIT_FAILURE;
        }
    }
    
    /* Wait for all threads to complete */
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    /* Print the final counter value (should match the expected value) */
    printf("Final counter value WITH mutex: %ld\n", shared_counter);
    printf("This value is %s the expected value\n", 
           (shared_counter == NUM_THREADS * ITERATIONS) ? "equal to" : "LESS THAN");
    
    /* Clean up the mutex when we're done with it */
    pthread_mutex_destroy(&counter_mutex);
    
    return EXIT_SUCCESS;
}

/**
 * Thread function that increments the shared counter WITHOUT mutex protection.
 * This is vulnerable to race conditions.
 */
void* increment_without_mutex(void* arg) {
    int i;
    
    for (i = 0; i < ITERATIONS; i++) {
        /* 
         * UNSAFE: This is a non-atomic operation composed of three steps:
         * 1. Read the current value of shared_counter
         * 2. Add 1 to the value
         * 3. Write the result back to shared_counter
         * 
         * If another thread intervenes between these steps, we get a race condition.
         */
        shared_counter++;
        
        /* 
         * Optional: Add a small sleep to make race conditions more likely
         * Uncomment this in a real demonstration if needed
         */
        // if (i % 10000 == 0) usleep(1);
    }
    
    return NULL;
}

/**
 * Thread function that increments the shared counter WITH mutex protection.
 * This prevents race conditions.
 */
void* increment_with_mutex(void* arg) {
    int i;
    
    for (i = 0; i < ITERATIONS; i++) {
        /* 
         * CRITICAL SECTION BEGIN:
         * Acquire the mutex lock before accessing the shared resource
         */
        pthread_mutex_lock(&counter_mutex);
        
        /* 
         * SAFE: This operation is now protected by the mutex.
         * Only one thread can execute this code at a time.
         */
        shared_counter++;
        
        /* 
         * CRITICAL SECTION END:
         * Release the mutex lock so other threads can access the shared resource
         */
        pthread_mutex_unlock(&counter_mutex);
        
        /* 
         * Optional: Add a small sleep outside the critical section
         * (This doesn't affect correctness but can change the interleaving pattern)
         */
        // if (i % 10000 == 0) usleep(1);
    }
    
    return NULL;
}

/**
 * EXPLANATION:
 * 
 * Race Conditions and Mutex Synchronization:
 * 
 * 1. What is a Race Condition?
 *    - A race condition occurs when multiple threads access shared data concurrently
 *    - The final outcome depends on the relative timing of the threads (non-deterministic)
 *    - It happens because operations that seem atomic actually involve multiple steps
 * 
 * 2. The Counter Increment Problem:
 *    - counter++ involves three steps: read value, add 1, write value
 *    - If Thread A reads the value, then Thread B reads the same value before
 *      Thread A has written back, both will increment from the same base value
 *    - This leads to lost updates and incorrect results
 * 
 * 3. Mutex Basics:
 *    - Mutex = MUTual EXclusion
 *    - Acts like a lock for a critical section of code
 *    - Only one thread can hold the lock at a time
 *    - Other threads must wait until the lock is released
 * 
 * 4. Mutex Operations:
 *    - pthread_mutex_init: Initialize a mutex before using it
 *    - pthread_mutex_lock: Acquire the lock (blocks if already locked)
 *    - pthread_mutex_unlock: Release the lock
 *    - pthread_mutex_destroy: Clean up mutex resources when done
 * 
 * 5. Critical Sections:
 *    - The code between lock and unlock is a "critical section"
 *    - Should be as small as possible to maintain concurrency
 *    - Only protect the actual shared data access, not unrelated operations
 * 
 * 6. Best Practices:
 *    - Always initialize mutexes before use
 *    - Always destroy mutexes when done
 *    - Keep critical sections small
 *    - Always unlock mutexes, even in error paths (consider pthread_cleanup_push)
 *    - Be aware of potential deadlocks (covered in future examples)
 *    - Consider using lock-free data structures for high-performance applications
 * 
 * 7. Performance Considerations:
 *    - Mutexes have overhead (acquire/release operations)
 *    - Too many locks can reduce concurrency
 *    - Fine-grained locking (multiple mutexes for different data) can improve performance
 *    - Mutex contention (threads waiting for locks) can limit scalability
 */