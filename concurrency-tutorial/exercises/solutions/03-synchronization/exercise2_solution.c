/*
 * SOLUTION: Exercise 2 - Mutex-Protected Counter
 *
 * This solution fixes the race condition from Exercise 1 by using a mutex
 * to protect the shared counter. It demonstrates proper mutex usage and
 * ensures the counter reaches the correct value every time.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 10
#define INCREMENTS_PER_THREAD 100000

// Global shared counter (now will be protected by mutex)
int shared_counter = 0;

// Mutex to protect the counter
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Thread function that increments the shared counter
 * WITH mutex protection
 */
void* increment_counter(void* arg) {
    int thread_id = (int)(long)arg;

    printf("Thread %d: Starting increments\n", thread_id);

    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        // CRITICAL SECTION BEGINS
        // Lock the mutex before accessing shared data
        pthread_mutex_lock(&counter_mutex);

        // Only one thread can be here at a time
        // This increment is now effectively atomic
        shared_counter++;

        // Unlock the mutex after modifying shared data
        pthread_mutex_unlock(&counter_mutex);
        // CRITICAL SECTION ENDS

        // Note: We keep the critical section as small as possible
        // for better performance
    }

    printf("Thread %d: Completed %d increments\n", thread_id, INCREMENTS_PER_THREAD);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int expected = NUM_THREADS * INCREMENTS_PER_THREAD;
    struct timespec start, end;

    printf("Starting mutex-protected counter demonstration...\n");
    printf("Expected final value: %d\n\n", expected);

    // Record start time
    clock_gettime(CLOCK_MONOTONIC, &start);

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

    // Record end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    // Calculate results
    int lost = expected - shared_counter;
    double percent_lost = (lost * 100.0) / expected;

    // Display results
    printf("\n===== RESULTS =====\n");
    printf("Final counter value: %d\n", shared_counter);
    printf("Expected value: %d\n", expected);
    printf("Lost updates: %d (%.2f%%)\n", lost, percent_lost);
    printf("Execution time: %.3f seconds\n", elapsed);

    if (shared_counter == expected) {
        printf("SUCCESS: Counter is correct!\n");
    } else {
        printf("ERROR: Counter is incorrect!\n");
    }

    // Clean up the mutex
    pthread_mutex_destroy(&counter_mutex);

    printf("\nMutexes guarantee correctness at the cost of some performance.\n");

    return 0;
}

/*
 * EXPLANATION OF MUTEX PROTECTION:
 *
 * 1. WHAT IS A MUTEX?
 *    - Mutex = "Mutual Exclusion"
 *    - A lock that only one thread can hold at a time
 *    - Protects critical sections of code
 *    - Ensures atomic access to shared resources
 *
 * 2. HOW MUTEXES WORK:
 *    - pthread_mutex_lock(): Acquires the lock (blocks if already held)
 *    - pthread_mutex_unlock(): Releases the lock
 *    - Only the thread that locked can unlock
 *    - Other threads wait in a queue
 *
 * 3. CRITICAL SECTION:
 *    The code between lock() and unlock() is the critical section.
 *    Only one thread can execute it at a time.
 *
 *    pthread_mutex_lock(&mutex);
 *    // <-- Critical section starts
 *    shared_counter++;
 *    // <-- Critical section ends
 *    pthread_mutex_unlock(&mutex);
 *
 * 4. WHY IT WORKS:
 *    Thread A                    Thread B
 *    --------                    --------
 *    lock(mutex)
 *    counter++ (gets 100->101)
 *                                lock(mutex) <-- BLOCKS, waits for A
 *    unlock(mutex)
 *                                <-- NOW gets lock
 *                                counter++ (gets 101->102)
 *                                unlock(mutex)
 *
 * 5. MUTEX INITIALIZATION:
 *    Two ways to initialize:
 *
 *    a) Static initialization (used in this solution):
 *       pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 *
 *    b) Dynamic initialization:
 *       pthread_mutex_t mutex;
 *       pthread_mutex_init(&mutex, NULL);
 *       ...
 *       pthread_mutex_destroy(&mutex);
 *
 * 6. PERFORMANCE TRADEOFF:
 *    - Mutexes add overhead (lock/unlock operations)
 *    - Threads must wait for each other (serialization)
 *    - Execution is slower than Exercise 1
 *    - BUT: Correctness is guaranteed!
 *
 *    Speed vs Correctness: Always choose correctness!
 *
 * 7. BEST PRACTICES:
 *    a) Keep critical sections small
 *       - Only protect what needs protecting
 *       - Don't do I/O or sleep while holding lock
 *       - Minimize time between lock and unlock
 *
 *    b) Always unlock what you lock
 *       - Consider using cleanup handlers for error paths
 *       - Unlock in reverse order when using multiple mutexes
 *
 *    c) Avoid deadlock
 *       - Don't lock same mutex twice (use recursive mutex if needed)
 *       - Establish global lock ordering
 *
 *    d) Check return values
 *       - pthread_mutex_lock() can fail
 *       - pthread_mutex_unlock() can fail
 *
 * 8. COMMON MISTAKES:
 *    - Forgetting to unlock (causes deadlock)
 *    - Unlocking in wrong order
 *    - Locking in one function, unlocking in another
 *    - Accessing shared data outside critical section
 *    - Making critical section too large
 *
 * 9. DEBUGGING TIPS:
 *    - Use valgrind --tool=helgrind to detect issues
 *    - Use ThreadSanitizer: -fsanitize=thread
 *    - Add debug prints (but not in critical section!)
 *    - Test with different thread counts
 *
 * 10. ALTERNATIVES TO MUTEXES:
 *     - Atomic operations: __atomic_add_fetch() for simple cases
 *     - Read-write locks: pthread_rwlock_t for read-heavy workloads
 *     - Spinlocks: For very short critical sections
 *     - Lock-free algorithms: For advanced cases
 *
 * COMPARISON WITH EXERCISE 1:
 * Exercise 1 (no mutex):
 *   - Fast but incorrect
 *   - Lost updates (10-30%)
 *   - Non-deterministic results
 *   - BUG!
 *
 * Exercise 2 (with mutex):
 *   - Slower but correct
 *   - No lost updates (0%)
 *   - Deterministic results (always 1,000,000)
 *   - Correct!
 *
 * KEY TAKEAWAY:
 * Mutexes are the fundamental building block for protecting shared data
 * in multithreaded programs. They guarantee correctness by ensuring only
 * one thread at a time can access the critical section.
 */
