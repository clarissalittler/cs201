/*
 * Exercise 5: Barrier Synchronization
 * Difficulty: 🔴 Advanced
 *
 * OBJECTIVE:
 * Create a reusable barrier that synchronizes multiple threads at specific
 * points. All threads must reach the barrier before any can proceed to the
 * next phase. The barrier should be reusable across multiple phases.
 *
 * REQUIREMENTS:
 * 1. Implement a custom barrier structure with:
 *    - Mutex for protecting barrier state
 *    - Condition variable for signaling
 *    - Count of threads that have arrived
 *    - Total number of threads expected
 *    - Phase counter (for reusability)
 * 2. Create 6 worker threads that go through 3 phases
 * 3. In each phase:
 *    - Threads do independent work (simulated)
 *    - All threads must reach barrier before any proceed
 *    - Last thread to arrive releases all waiting threads
 * 4. Barrier must be reusable (not create new barriers each phase)
 * 5. Print detailed messages showing synchronization
 *
 * EXPECTED BEHAVIOR:
 * - In each phase, threads arrive at barrier at different times
 * - Early arrivers wait for late arrivers
 * - Once all threads arrive, all are released simultaneously
 * - Threads proceed to next phase together
 * - All threads complete all phases
 *
 * SAMPLE OUTPUT:
 * Starting Barrier Synchronization demonstration...
 * 6 threads, 3 phases
 *
 * === Phase 1 ===
 * Thread 0: Working on phase 1...
 * Thread 1: Working on phase 1...
 * ...
 * Thread 0: Finished work, waiting at barrier (1/6)
 * Thread 2: Finished work, waiting at barrier (2/6)
 * Thread 1: Finished work, waiting at barrier (3/6)
 * Thread 4: Finished work, waiting at barrier (4/6)
 * Thread 3: Finished work, waiting at barrier (5/6)
 * Thread 5: Finished work, waiting at barrier (6/6)
 * Thread 5: Last to arrive, releasing all threads
 * Thread 0: Released from barrier, entering phase 2
 * Thread 1: Released from barrier, entering phase 2
 * ...
 *
 * === Phase 2 ===
 * ...
 *
 * ===== ALL PHASES COMPLETE =====
 * SUCCESS: All threads synchronized correctly!
 *
 * BARRIER STRUCTURE:
 * typedef struct {
 *     pthread_mutex_t mutex;
 *     pthread_cond_t cv;
 *     int count;           // Threads currently at barrier
 *     int total_threads;   // Total threads expected
 *     int phase;           // Current phase (for reusability)
 * } barrier_t;
 *
 * BARRIER ALGORITHM:
 * void barrier_wait(barrier_t* barrier, int thread_id) {
 *     pthread_mutex_lock(&barrier->mutex);
 *
 *     int my_phase = barrier->phase;
 *     barrier->count++;
 *
 *     if (barrier->count == barrier->total_threads) {
 *         // Last thread to arrive
 *         barrier->count = 0;
 *         barrier->phase++;
 *         pthread_cond_broadcast(&barrier->cv);  // Wake all
 *     } else {
 *         // Not last, wait for others
 *         while (barrier->phase == my_phase) {
 *             pthread_cond_wait(&barrier->cv, &barrier->mutex);
 *         }
 *     }
 *
 *     pthread_mutex_unlock(&barrier->mutex);
 * }
 *
 * HINTS:
 * - Each thread must save the current phase before incrementing count
 * - Use pthread_cond_broadcast() not signal() (need to wake ALL threads)
 * - The while loop prevents issues with spurious wakeups
 * - Use random sleep times to simulate varying work durations
 * - The last thread arriving is responsible for releasing others
 *
 * WHY PHASE COUNTER?
 * The phase counter makes the barrier reusable. Without it, threads might
 * wake up from a previous phase and proceed incorrectly. The phase ensures
 * threads wait for the correct synchronization point.
 *
 * LEARNING GOALS:
 * - Implement a synchronization primitive from scratch
 * - Understand barrier synchronization patterns
 * - Use condition variables with broadcast
 * - Handle reusable synchronization constructs
 * - See real-world use case: parallel algorithms with phases
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// TODO: Define constants
// #define NUM_THREADS 6
// #define NUM_PHASES 3

// TODO: Define barrier structure
// typedef struct {
//     pthread_mutex_t mutex;
//     pthread_cond_t cv;
//     int count;           // Number of threads at barrier
//     int total_threads;   // Total threads expected
//     int phase;           // Current phase number
// } barrier_t;

// TODO: Declare global barrier
// barrier_t barrier;

// TODO: Implement barrier initialization
// void barrier_init(barrier_t* b, int n) {
//     pthread_mutex_init(&b->mutex, NULL);
//     pthread_cond_init(&b->cv, NULL);
//     b->count = 0;
//     b->total_threads = n;
//     b->phase = 0;
// }

// TODO: Implement barrier wait function
// void barrier_wait(barrier_t* b, int thread_id, int phase_num) {
//     // Lock the barrier mutex
//     pthread_mutex_lock(&b->mutex);
//
//     // Save current phase
//     int my_phase = b->phase;
//
//     // Increment count of arrived threads
//     b->count++;
//     printf("Thread %d: Waiting at barrier (%d/%d) for phase %d\n",
//            thread_id, b->count, b->total_threads, phase_num);
//
//     // Check if this is the last thread
//     if (b->count == b->total_threads) {
//         // Last thread - release all waiting threads
//         printf("Thread %d: Last to arrive, releasing all threads\n", thread_id);
//         b->count = 0;
//         b->phase++;  // Advance to next phase
//         pthread_cond_broadcast(&b->cv);  // Wake all waiting threads
//     } else {
//         // Not last - wait until phase changes
//         while (b->phase == my_phase) {
//             pthread_cond_wait(&b->cv, &b->mutex);
//         }
//         printf("Thread %d: Released from barrier\n", thread_id);
//     }
//
//     pthread_mutex_unlock(&b->mutex);
// }

// TODO: Implement barrier cleanup
// void barrier_destroy(barrier_t* b) {
//     pthread_mutex_destroy(&b->mutex);
//     pthread_cond_destroy(&b->cv);
// }

// TODO: Implement worker thread function
// void* worker(void* arg) {
//     int thread_id = (int)(long)arg;
//
//     for (int phase = 1; phase <= NUM_PHASES; phase++) {
//         // TODO: Work phase
//         // printf("Thread %d: Working on phase %d...\n", thread_id, phase);
//         // usleep(rand() % 200000);  // Simulate work (0-200ms)
//         // printf("Thread %d: Completed work for phase %d\n", thread_id, phase);
//
//         // TODO: Wait at barrier
//         // barrier_wait(&barrier, thread_id, phase);
//
//         // TODO: Small delay before next phase
//         // usleep(10000);
//     }
//
//     printf("Thread %d: Completed all phases\n", thread_id);
//     return NULL;
// }

int main() {
    // TODO: Declare thread array
    // pthread_t threads[NUM_THREADS];

    printf("Starting Barrier Synchronization demonstration...\n");
    // TODO: Uncomment after defining constants
    // printf("%d threads, %d phases\n\n", NUM_THREADS, NUM_PHASES);

    // TODO: Initialize barrier
    // barrier_init(&barrier, NUM_THREADS);

    // TODO: Create worker threads
    // printf("Creating threads...\n\n");
    // for (int i = 0; i < NUM_THREADS; i++) {
    //     pthread_create(&threads[i], NULL, worker, (void*)(long)i);
    // }

    // TODO: Wait for all threads to complete
    // for (int i = 0; i < NUM_THREADS; i++) {
    //     pthread_join(threads[i], NULL);
    // }

    // TODO: Cleanup
    // barrier_destroy(&barrier);

    printf("\n===== ALL PHASES COMPLETE =====\n");
    printf("SUCCESS: All threads synchronized correctly!\n");
    printf("\nBarrier synchronization ensures all threads reach a point\n");
    printf("before any proceed to the next phase.\n");

    return 0;
}

/*
 * REAL-WORLD APPLICATIONS OF BARRIERS:
 *
 * 1. Parallel Numerical Simulations:
 *    - Multiple phases of computation
 *    - Each phase depends on results from previous phase
 *    - All threads must finish phase N before any start phase N+1
 *
 * 2. Parallel Matrix Operations:
 *    - Distribute rows/columns to threads
 *    - Synchronize after each computation step
 *    - Ensure consistency before next operation
 *
 * 3. Parallel Sorting:
 *    - Merge sort with parallel merge steps
 *    - Barrier between merge phases
 *
 * 4. Game Engines:
 *    - Physics update phase
 *    - Rendering phase
 *    - Barrier between phases
 *
 * 5. Machine Learning:
 *    - Parallel gradient computation
 *    - Barrier before weight updates
 *    - Synchronize between training iterations
 *
 * PTHREAD BARRIER (Alternative):
 * Note: POSIX provides pthread_barrier_t for this purpose.
 * This exercise implements it manually for educational purposes.
 *
 * Using POSIX barriers:
 * pthread_barrier_t barrier;
 * pthread_barrier_init(&barrier, NULL, NUM_THREADS);
 * pthread_barrier_wait(&barrier);  // In each thread
 * pthread_barrier_destroy(&barrier);
 */
