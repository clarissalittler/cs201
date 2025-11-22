/*
 * SOLUTION: Exercise 3 - Producer-Consumer Queue
 *
 * This solution implements a classic producer-consumer pattern using a
 * bounded buffer. It demonstrates the use of mutexes for mutual exclusion
 * and condition variables for thread coordination.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZE 10
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 2
#define ITEMS_PER_PRODUCER 20
#define TOTAL_ITEMS (NUM_PRODUCERS * ITEMS_PER_PRODUCER)

// Circular buffer for items
int buffer[BUFFER_SIZE];
int count = 0;           // Number of items currently in buffer
int in_index = 0;        // Index where producers insert
int out_index = 0;       // Index where consumers remove
int next_item = 1;       // Next item number to produce
int items_consumed = 0;  // Total items consumed so far

// Synchronization primitives
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;   // Signaled when buffer not full
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;  // Signaled when buffer not empty

/*
 * Producer thread function
 * Produces items and adds them to the buffer
 */
void* producer(void* arg) {
    int producer_id = (int)(long)arg;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        // Lock the mutex before accessing shared state
        pthread_mutex_lock(&mutex);

        // Wait while buffer is full
        // IMPORTANT: Use WHILE loop, not IF (handles spurious wakeups)
        while (count == BUFFER_SIZE) {
            printf("Producer %d: Buffer full, waiting...\n", producer_id);
            // Wait releases mutex, reacquires when woken up
            pthread_cond_wait(&not_full, &mutex);
            printf("Producer %d: Woke up, buffer has space\n", producer_id);
        }

        // Produce item and add to buffer
        int item = next_item++;
        buffer[in_index] = item;
        in_index = (in_index + 1) % BUFFER_SIZE;  // Circular buffer
        count++;

        printf("Producer %d: Produced item %d, buffer count: %d\n",
               producer_id, item, count);

        // Signal that buffer is not empty
        pthread_cond_signal(&not_empty);

        // Unlock mutex
        pthread_mutex_unlock(&mutex);

        // Simulate variable production time
        usleep(rand() % 10000);
    }

    printf("Producer %d: Finished producing %d items\n",
           producer_id, ITEMS_PER_PRODUCER);
    return NULL;
}

/*
 * Consumer thread function
 * Removes items from the buffer and consumes them
 */
void* consumer(void* arg) {
    int consumer_id = (int)(long)arg;

    while (1) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Wait while buffer is empty AND not all items have been consumed
        while (count == 0 && items_consumed < TOTAL_ITEMS) {
            printf("Consumer %d: Buffer empty, waiting...\n", consumer_id);
            pthread_cond_wait(&not_empty, &mutex);
            printf("Consumer %d: Woke up\n", consumer_id);
        }

        // Check if we're done (all items consumed)
        if (items_consumed >= TOTAL_ITEMS) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Check again if buffer is empty (might have been consumed by another thread)
        if (count == 0) {
            pthread_mutex_unlock(&mutex);
            continue;
        }

        // Consume item from buffer
        int item = buffer[out_index];
        out_index = (out_index + 1) % BUFFER_SIZE;  // Circular buffer
        count--;
        items_consumed++;

        printf("Consumer %d: Consumed item %d, buffer count: %d (total: %d/%d)\n",
               consumer_id, item, count, items_consumed, TOTAL_ITEMS);

        // Signal that buffer is not full
        pthread_cond_signal(&not_full);

        // Unlock mutex
        pthread_mutex_unlock(&mutex);

        // Simulate variable consumption time
        usleep(rand() % 15000);
    }

    printf("Consumer %d: Finished consuming\n", consumer_id);
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    printf("Starting Producer-Consumer demonstration...\n");
    printf("Buffer capacity: %d\n", BUFFER_SIZE);
    printf("Producers: %d (%d items each)\n", NUM_PRODUCERS, ITEMS_PER_PRODUCER);
    printf("Consumers: %d\n", NUM_CONSUMERS);
    printf("Total items: %d\n\n", TOTAL_ITEMS);

    // Seed random number generator
    srand(time(NULL));

    // Create producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, producer, (void*)(long)i);
    }

    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void*)(long)i);
    }

    // Wait for all producers to finish
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    printf("\n--- All producers finished ---\n\n");

    // Wake up all consumers in case they're waiting
    pthread_cond_broadcast(&not_empty);

    // Wait for all consumers to finish
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Print final results
    printf("\n===== RESULTS =====\n");
    printf("Total items produced: %d\n", TOTAL_ITEMS);
    printf("Total items consumed: %d\n", items_consumed);

    if (items_consumed == TOTAL_ITEMS) {
        printf("SUCCESS: All items accounted for!\n");
    } else {
        printf("ERROR: Item count mismatch!\n");
    }

    // Cleanup
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}

/*
 * EXPLANATION OF PRODUCER-CONSUMER PATTERN:
 *
 * 1. THE PROBLEM:
 *    - Producers generate data at one rate
 *    - Consumers process data at another rate
 *    - Need a buffer to handle rate mismatches
 *    - Buffer has limited capacity (bounded buffer)
 *
 * 2. SYNCHRONIZATION REQUIREMENTS:
 *    a) Mutual exclusion: Only one thread modifies buffer at a time
 *    b) Producer waits when buffer is FULL
 *    c) Consumer waits when buffer is EMPTY
 *    d) Wake up waiting threads when conditions change
 *
 * 3. CONDITION VARIABLES:
 *    Condition variables allow threads to wait for conditions and be
 *    notified when conditions become true.
 *
 *    pthread_cond_wait(&cond, &mutex):
 *      - Atomically releases mutex and blocks
 *      - Reacquires mutex when woken up
 *      - Must be called while holding the mutex
 *
 *    pthread_cond_signal(&cond):
 *      - Wakes up ONE waiting thread
 *      - Use when any one waiter can proceed
 *
 *    pthread_cond_broadcast(&cond):
 *      - Wakes up ALL waiting threads
 *      - Use when multiple waiters might proceed
 *
 * 4. WHY WHILE LOOP, NOT IF?
 *    Always use while loop with condition variables:
 *
 *    WRONG:
 *    if (count == BUFFER_SIZE) {
 *        pthread_cond_wait(&not_full, &mutex);
 *    }
 *
 *    CORRECT:
 *    while (count == BUFFER_SIZE) {
 *        pthread_cond_wait(&not_full, &mutex);
 *    }
 *
 *    Reasons:
 *    - Spurious wakeups: Thread can wake up without signal
 *    - Multiple waiters: Another thread might grab resource first
 *    - Condition might change between signal and wakeup
 *
 * 5. PRODUCER LOGIC:
 *    lock(mutex)
 *    while (buffer_full)
 *        wait(not_full, mutex)    // Wait for space
 *    add_item_to_buffer()
 *    signal(not_empty)            // Tell consumers item available
 *    unlock(mutex)
 *
 * 6. CONSUMER LOGIC:
 *    lock(mutex)
 *    while (buffer_empty)
 *        wait(not_empty, mutex)   // Wait for item
 *    remove_item_from_buffer()
 *    signal(not_full)             // Tell producers space available
 *    unlock(mutex)
 *
 * 7. CIRCULAR BUFFER:
 *    - Fixed-size array
 *    - Two indices: in (producer) and out (consumer)
 *    - Wrap around using modulo: (index + 1) % SIZE
 *    - Efficient reuse of buffer space
 *
 * 8. TERMINATION STRATEGY:
 *    - Producers: Complete after producing all items
 *    - Consumers: Check if all items consumed
 *    - Main thread: Broadcast to wake up waiting consumers
 *    - Consumers: Exit when condition met
 *
 * 9. REAL-WORLD APPLICATIONS:
 *    - Web servers: Request queue
 *    - Video streaming: Frame buffer
 *    - Logging systems: Log message queue
 *    - Print spoolers: Print job queue
 *    - Task schedulers: Work queue
 *
 * 10. PERFORMANCE CONSIDERATIONS:
 *     - Buffer size affects performance
 *     - Too small: Lots of waiting
 *     - Too large: Memory waste
 *     - Optimal size depends on production/consumption rates
 *
 * 11. COMMON MISTAKES:
 *     - Using if instead of while
 *     - Forgetting to hold mutex during cond_wait
 *     - Signaling wrong condition variable
 *     - Not handling termination properly
 *     - Deadlock from not signaling
 *
 * 12. EXTENSIONS:
 *     - Priority items
 *     - Multiple buffers
 *     - Batch processing
 *     - Non-blocking operations
 *     - Timed waits
 *
 * KEY CONCEPTS:
 * - Bounded buffer: Fixed-size queue
 * - Condition variable: Wait for conditions
 * - Signal vs Broadcast: One vs all waiters
 * - While loop: Handle spurious wakeups
 * - Graceful termination: Coordinate shutdown
 */
