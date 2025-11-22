/*
 * SOLUTION: Exercise 1 - Race Condition Demonstration
 *
 * This solution demonstrates race conditions by having multiple threads
 * increment a shared counter without synchronization. The program shows
 * the "lost update" problem that occurs when operations are not atomic.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 10
#define INCREMENTS_PER_THREAD 100000

// Global shared counter (NOT protected - this is intentional for demonstration)
int shared_counter = 0;

/*
 * Thread function that increments the shared counter
 * WITHOUT any synchronization protection
 */
void* increment_counter(void* arg) {
    int thread_id = (int)(long)arg;

    printf("Thread %d: Starting increments\n", thread_id);

    // Increment counter many times without protection
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        // This operation is NOT atomic - it consists of:
        // 1. Load shared_counter from memory to register
        // 2. Add 1 to register value
        // 3. Store register value back to memory
        shared_counter++;

        // Occasionally yield to increase likelihood of race conditions
        if (i % 10000 == 0) {
            usleep(1);
        }
    }

    printf("Thread %d: Completed %d increments\n", thread_id, INCREMENTS_PER_THREAD);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int expected = NUM_THREADS * INCREMENTS_PER_THREAD;

    printf("Starting race condition demonstration...\n");
    printf("Expected final value: %d\n\n", expected);

    // Create all threads
    for (int i = 0; i < NUM_THREADS; i++) {
        int result = pthread_create(&threads[i], NULL, increment_counter,
                                    (void*)(long)i);
        if (result != 0) {
            printf("Error creating thread %d: %d\n", i, result);
            return EXIT_FAILURE;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Calculate and display results
    int lost = expected - shared_counter;
    double percent_lost = (lost * 100.0) / expected;

    printf("\n===== RESULTS =====\n");
    printf("Final counter value: %d\n", shared_counter);
    printf("Expected value: %d\n", expected);
    printf("Lost updates: %d (%.2f%%)\n", lost, percent_lost);

    printf("\nThis demonstrates why synchronization is necessary!\n");
    printf("Exercise 2 will show how to fix this with mutexes.\n");

    return 0;
}

/*
 * EXPLANATION OF THE RACE CONDITION:
 *
 * 1. THE PROBLEM:
 *    The statement "shared_counter++" looks atomic but is actually three
 *    separate machine instructions:
 *    a) LOAD: Read shared_counter from memory into CPU register
 *    b) ADD:  Increment the value in the register
 *    c) STORE: Write the register value back to memory
 *
 * 2. RACE SCENARIO (Example):
 *    Initial value: shared_counter = 100
 *
 *    Thread A                    Thread B
 *    --------                    --------
 *    LOAD (gets 100)
 *                                LOAD (gets 100)
 *    ADD (now has 101)
 *                                ADD (now has 101)
 *    STORE (writes 101)
 *                                STORE (writes 101)
 *
 *    Result: shared_counter = 101
 *    Expected: shared_counter = 102
 *    Lost updates: 1
 *
 * 3. WHY IT HAPPENS:
 *    - Threads share the same memory space
 *    - Thread scheduling is unpredictable
 *    - No synchronization prevents interleaving
 *    - Multiple threads can read the same old value
 *    - Later writes overwrite earlier writes
 *
 * 4. OBSERVING THE EFFECT:
 *    - Run this program multiple times
 *    - You'll get different results each time
 *    - Typical loss: 10-30% of updates
 *    - More threads = more contention = more losses
 *    - Faster CPUs may show more losses (more parallelism)
 *
 * 5. WHAT MAKES IT WORSE:
 *    - More threads competing for the same variable
 *    - More iterations per thread
 *    - Multi-core processors (true parallelism)
 *    - Compiler optimizations (register caching)
 *
 * 6. THIS IS A BUG!
 *    In production code, race conditions like this are:
 *    - Hard to reproduce
 *    - Hard to debug
 *    - Cause intermittent failures
 *    - Lead to data corruption
 *    - Violate program correctness
 *
 * 7. THE FIX:
 *    Exercise 2 shows how to fix this with mutexes to ensure
 *    only one thread at a time can increment the counter.
 *
 * KEY CONCEPTS:
 * - Race condition: Multiple threads accessing shared data without coordination
 * - Critical section: Code that accesses shared data
 * - Atomic operation: Operation that completes without interruption
 * - Lost update: Update that gets overwritten before taking effect
 * - Data race: Concurrent access to shared data with at least one write
 *
 * TESTING TIPS:
 * - Run multiple times to see variation
 * - Try different NUM_THREADS values
 * - Try different INCREMENTS_PER_THREAD values
 * - Remove usleep() to see if it affects results
 * - Use tools like ThreadSanitizer: gcc -fsanitize=thread
 */
