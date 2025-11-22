/*
 * SOLUTION: Exercise 5 - Barrier Synchronization
 *
 * This solution implements a reusable barrier that synchronizes multiple
 * threads at specific points. It demonstrates how to build a custom
 * synchronization primitive using mutexes and condition variables.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_THREADS 6
#define NUM_PHASES 3

/*
 * Barrier structure
 * Contains all state needed for barrier synchronization
 */
typedef struct {
    pthread_mutex_t mutex;   // Protects barrier state
    pthread_cond_t cv;       // For signaling waiting threads
    int count;               // Number of threads currently at barrier
    int total_threads;       // Total threads expected
    int phase;               // Current phase (for reusability)
} barrier_t;

// Global barrier
barrier_t barrier;

/*
 * Initialize a barrier
 * Must be called before barrier is used
 */
void barrier_init(barrier_t* b, int n) {
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cv, NULL);
    b->count = 0;
    b->total_threads = n;
    b->phase = 0;
}

/*
 * Wait at barrier
 * All threads must call this before any can proceed
 */
void barrier_wait(barrier_t* b, int thread_id, int phase_num) {
    // Lock the barrier mutex
    pthread_mutex_lock(&b->mutex);

    // Save the current phase
    // This is crucial for reusability!
    int my_phase = b->phase;

    // Increment count of threads at barrier
    b->count++;
    printf("Thread %d: Waiting at barrier (%d/%d) for phase %d\n",
           thread_id, b->count, b->total_threads, phase_num);

    // Check if this is the last thread to arrive
    if (b->count == b->total_threads) {
        // Last thread to arrive - release all waiting threads
        printf("Thread %d: Last to arrive, releasing all threads from phase %d\n",
               thread_id, phase_num);

        // Reset count for next use
        b->count = 0;

        // Advance to next phase (makes barrier reusable)
        b->phase++;

        // Wake up ALL waiting threads
        pthread_cond_broadcast(&b->cv);
    } else {
        // Not the last thread - wait until phase changes
        // Use WHILE loop to handle spurious wakeups
        while (b->phase == my_phase) {
            pthread_cond_wait(&b->cv, &b->mutex);
        }
        printf("Thread %d: Released from barrier (phase %d)\n",
               thread_id, phase_num);
    }

    // Unlock the mutex
    pthread_mutex_unlock(&b->mutex);
}

/*
 * Destroy a barrier
 * Cleanup resources
 */
void barrier_destroy(barrier_t* b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cv);
}

/*
 * Worker thread function
 * Each thread goes through multiple phases with barrier synchronization
 */
void* worker(void* arg) {
    int thread_id = (int)(long)arg;

    for (int phase = 1; phase <= NUM_PHASES; phase++) {
        // ========== WORK PHASE ==========
        printf("Thread %d: Working on phase %d...\n", thread_id, phase);

        // Simulate variable work time
        usleep(rand() % 200000);  // 0-200ms

        printf("Thread %d: Completed work for phase %d\n", thread_id, phase);

        // ========== BARRIER SYNCHRONIZATION ==========
        // Wait for all threads to complete this phase
        barrier_wait(&barrier, thread_id, phase);

        // ========== POST-BARRIER ==========
        // All threads reach here together
        // Small delay before starting next phase
        usleep(10000);

        if (phase < NUM_PHASES) {
            printf("Thread %d: Entering phase %d\n", thread_id, phase + 1);
        }
    }

    printf("Thread %d: Completed all %d phases\n", thread_id, NUM_PHASES);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    printf("Starting Barrier Synchronization demonstration...\n");
    printf("%d threads, %d phases\n", NUM_THREADS, NUM_PHASES);
    printf("All threads must complete each phase before any proceed to next\n\n");

    // Seed random number generator
    srand(time(NULL));

    // Initialize the barrier
    barrier_init(&barrier, NUM_THREADS);

    // Create all worker threads
    printf("Creating threads...\n\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, (void*)(long)i);
    }

    // Wait for all threads to complete all phases
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup barrier
    barrier_destroy(&barrier);

    printf("\n===== ALL PHASES COMPLETE =====\n");
    printf("SUCCESS: All threads synchronized correctly!\n");
    printf("\nBarrier synchronization ensures all threads reach a point\n");
    printf("before any proceed to the next phase.\n");

    return 0;
}

/*
 * EXPLANATION OF BARRIER SYNCHRONIZATION:
 *
 * 1. WHAT IS A BARRIER?
 *    A synchronization point where all threads must arrive before any
 *    can proceed. Like a gate that only opens when everyone arrives.
 *
 *    Analogy: A group tour - tour guide waits for all tourists before
 *             moving to the next attraction.
 *
 * 2. WHY BARRIERS ARE NEEDED:
 *    - Parallel algorithms with phases
 *    - Each phase depends on results from previous phase
 *    - All threads must finish phase N before any start phase N+1
 *
 * 3. BARRIER OPERATION:
 *    Thread arrives at barrier:
 *    - If not last: Wait
 *    - If last: Wake all waiting threads
 *    - All threads proceed together
 *
 * 4. BARRIER STATE:
 *    - count: Number of threads currently at barrier
 *    - total_threads: Expected number of threads
 *    - phase: Current phase number (for reusability)
 *    - mutex: Protects count and phase
 *    - cv: For waiting and signaling
 *
 * 5. THE PHASE COUNTER (Critical for Reusability):
 *
 *    Without phase counter:
 *    Thread A: reaches barrier 1, waits
 *    Thread B: reaches barrier 1, is last, broadcasts
 *    Thread B: races ahead to barrier 2, broadcasts
 *    Thread A: wakes up, might see barrier 2 broadcast
 *    Thread A: proceeds incorrectly!
 *
 *    With phase counter:
 *    - Each thread saves current phase before waiting
 *    - Only proceeds when phase changes
 *    - Prevents confusion between barrier instances
 *
 * 6. WHY BROADCAST, NOT SIGNAL?
 *    - pthread_cond_signal() wakes ONE thread
 *    - pthread_cond_broadcast() wakes ALL threads
 *    - Barrier needs to release ALL waiting threads
 *    - Using signal() would only wake one thread at a time
 *
 * 7. WHY WHILE LOOP?
 *    while (b->phase == my_phase) {
 *        pthread_cond_wait(&b->cv, &b->mutex);
 *    }
 *
 *    Reasons:
 *    - Spurious wakeups: Thread can wake without signal
 *    - Race conditions: Phase might not have changed yet
 *    - Correctness: Ensures we proceed only when phase advances
 *
 * 8. BARRIER ALGORITHM STEP-BY-STEP:
 *
 *    Thread arrives:
 *    1. Lock mutex
 *    2. Save current phase number (my_phase)
 *    3. Increment count
 *    4. If count == total_threads:
 *       a. Reset count to 0
 *       b. Increment phase
 *       c. Broadcast to all waiters
 *    5. Else:
 *       a. While phase == my_phase: wait
 *    6. Unlock mutex
 *
 * 9. REAL-WORLD APPLICATIONS:
 *
 *    a) Parallel Numerical Simulations:
 *       for each iteration:
 *           - Compute local values
 *           - Barrier (wait for all)
 *           - Exchange boundary data
 *           - Barrier (wait for all)
 *           - Update based on neighbors
 *
 *    b) Parallel Matrix Operations:
 *       - Distribute matrix rows to threads
 *       - Each thread processes its rows
 *       - Barrier before next operation
 *       - Ensures consistency
 *
 *    c) Game Engines:
 *       Each frame:
 *       - Physics phase (all threads)
 *       - Barrier
 *       - Collision detection (all threads)
 *       - Barrier
 *       - Rendering (all threads)
 *
 *    d) Machine Learning:
 *       Each training iteration:
 *       - Compute gradients (parallel)
 *       - Barrier
 *       - Update weights (coordinated)
 *       - Barrier
 *
 *    e) Parallel Sorting:
 *       - Partition data to threads
 *       - Each thread sorts locally
 *       - Barrier
 *       - Merge results
 *
 * 10. PTHREAD BARRIER (Alternative):
 *     POSIX provides pthread_barrier_t:
 *
 *     pthread_barrier_t barrier;
 *     pthread_barrier_init(&barrier, NULL, NUM_THREADS);
 *
 *     // In each thread:
 *     pthread_barrier_wait(&barrier);
 *
 *     pthread_barrier_destroy(&barrier);
 *
 *     Our implementation shows how it works internally!
 *
 * 11. PERFORMANCE CONSIDERATIONS:
 *     - Barrier adds synchronization overhead
 *     - All threads must wait for slowest thread
 *     - Can limit scalability (Amdahl's Law)
 *     - Minimize barrier frequency
 *     - Maximize work between barriers
 *
 * 12. COMMON MISTAKES:
 *     - Forgetting to reset count
 *     - Not using phase counter (breaks reusability)
 *     - Using signal instead of broadcast
 *     - Using if instead of while
 *     - Not all threads calling barrier (deadlock!)
 *
 * 13. VARIATIONS:
 *
 *     a) Sense-Reversal Barrier:
 *        - Use boolean flag instead of phase counter
 *        - Flip flag each use: true→false→true
 *
 *     b) Tree Barrier:
 *        - Hierarchical wakeup
 *        - Better scalability for many threads
 *
 *     c) Fuzzy Barrier:
 *        - Allow some threads to proceed early
 *        - Trade accuracy for performance
 *
 *     d) Counted Barrier:
 *        - Variable number of threads each use
 *        - More flexible
 *
 * 14. DEBUGGING TIPS:
 *     - Print thread ID and phase at each step
 *     - Verify all threads reach each barrier
 *     - Check count resets properly
 *     - Test with different thread counts
 *     - Use helgrind or tsan to detect issues
 *
 * 15. COMPARISON WITH OTHER SYNCHRONIZATION:
 *
 *     Mutex:
 *     - Mutual exclusion
 *     - One thread at a time
 *     - Protects shared data
 *
 *     Condition Variable:
 *     - Wait for condition
 *     - Signal when condition true
 *     - Coordinate between threads
 *
 *     Barrier:
 *     - Synchronization point
 *     - All threads must arrive
 *     - Phase coordination
 *
 *     Semaphore:
 *     - Counting resource
 *     - Limited concurrency
 *     - Resource pools
 *
 * KEY TAKEAWAYS:
 * - Barriers synchronize threads at specific points
 * - All must arrive before any proceed
 * - Phase counter enables reusability
 * - Use broadcast to wake all waiters
 * - Essential for parallel algorithms with phases
 * - Can build complex primitives from basic ones (mutex + condvar)
 *
 * EDUCATIONAL VALUE:
 * This exercise shows how to:
 * - Build a synchronization primitive from scratch
 * - Use mutexes and condition variables together
 * - Handle reusable synchronization constructs
 * - Coordinate multiple threads at specific points
 * - Implement a pattern used in real parallel algorithms
 */
