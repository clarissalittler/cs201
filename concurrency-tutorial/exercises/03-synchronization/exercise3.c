/*
 * Exercise 3: Producer-Consumer Queue
 * Difficulty: 🟡 Intermediate
 *
 * OBJECTIVE:
 * Implement a bounded buffer where producer threads add items and consumer
 * threads remove them. Use mutexes and condition variables for proper
 * synchronization and thread coordination.
 *
 * REQUIREMENTS:
 * 1. Create a circular buffer (queue) with fixed capacity of 10 items
 * 2. Implement 3 producer threads that each produce 20 items
 * 3. Implement 2 consumer threads that consume all 60 items
 * 4. Use a mutex to protect the buffer
 * 5. Use condition variables to signal:
 *    - When buffer is not full (for producers)
 *    - When buffer is not empty (for consumers)
 * 6. Producers must wait when buffer is full
 * 7. Consumers must wait when buffer is empty
 * 8. Print detailed messages for all operations
 *
 * EXPECTED BEHAVIOR:
 * - All 60 items produced are eventually consumed
 * - No items are lost or duplicated
 * - Producers wait when buffer is full
 * - Consumers wait when buffer is empty
 * - Clean termination when all items are consumed
 *
 * SAMPLE OUTPUT:
 * Starting Producer-Consumer demonstration...
 * Buffer capacity: 10
 * Producers: 3 (20 items each)
 * Consumers: 2
 *
 * Producer 0: Produced item 1, buffer count: 1
 * Consumer 0: Consumed item 1, buffer count: 0
 * Producer 1: Produced item 2, buffer count: 1
 * Producer 0: Produced item 3, buffer count: 2
 * ...
 * Producer 0: Buffer full, waiting...
 * Consumer 1: Consumed item 10, buffer count: 9
 * Producer 0: Resumed, buffer has space
 * ...
 *
 * ===== RESULTS =====
 * Total produced: 60
 * Total consumed: 60
 * SUCCESS: All items accounted for!
 *
 * KEY PATTERNS:
 *
 * Producer Pattern:
 * pthread_mutex_lock(&mutex);
 * while (buffer_is_full) {
 *     pthread_cond_wait(&not_full, &mutex);
 * }
 * // Add item to buffer
 * pthread_cond_signal(&not_empty);
 * pthread_mutex_unlock(&mutex);
 *
 * Consumer Pattern:
 * pthread_mutex_lock(&mutex);
 * while (buffer_is_empty) {
 *     pthread_cond_wait(&not_empty, &mutex);
 * }
 * // Remove item from buffer
 * pthread_cond_signal(&not_full);
 * pthread_mutex_unlock(&mutex);
 *
 * HINTS:
 * - Use a circular buffer: int buffer[BUFFER_SIZE]
 * - Track: int count, int in_index, int out_index
 * - Declare: pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 * - Declare: pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
 * - Declare: pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
 * - Always use WHILE loop with condition variables (not IF)
 * - Signal condition variables while holding the mutex
 * - Producers increment a global item counter for unique items
 *
 * LEARNING GOALS:
 * - Use condition variables for thread coordination
 * - Implement producer-consumer pattern
 * - Understand bounded buffer management
 * - See the importance of while loops with pthread_cond_wait()
 * - Learn when to use signal vs broadcast
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// TODO: Define constants
// #define BUFFER_SIZE 10
// #define NUM_PRODUCERS 3
// #define NUM_CONSUMERS 2
// #define ITEMS_PER_PRODUCER 20
// #define TOTAL_ITEMS (NUM_PRODUCERS * ITEMS_PER_PRODUCER)

// TODO: Declare buffer and related variables
// int buffer[BUFFER_SIZE];
// int count = 0;           // Number of items in buffer
// int in_index = 0;        // Where producers insert
// int out_index = 0;       // Where consumers remove
// int next_item = 1;       // Next item number to produce
// int items_consumed = 0;  // Total items consumed

// TODO: Declare synchronization primitives
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
// pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

// TODO: Implement producer thread function
// void* producer(void* arg) {
//     int producer_id = (int)(long)arg;
//
//     for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
//         // TODO: Lock mutex
//         // pthread_mutex_lock(&mutex);
//
//         // TODO: Wait while buffer is full
//         // while (count == BUFFER_SIZE) {
//         //     printf("Producer %d: Buffer full, waiting...\n", producer_id);
//         //     pthread_cond_wait(&not_full, &mutex);
//         // }
//
//         // TODO: Produce item (add to buffer)
//         // int item = next_item++;
//         // buffer[in_index] = item;
//         // in_index = (in_index + 1) % BUFFER_SIZE;
//         // count++;
//         // printf("Producer %d: Produced item %d, buffer count: %d\n",
//         //        producer_id, item, count);
//
//         // TODO: Signal that buffer is not empty
//         // pthread_cond_signal(&not_empty);
//
//         // TODO: Unlock mutex
//         // pthread_mutex_unlock(&mutex);
//
//         // TODO: Simulate production time
//         // usleep(rand() % 10000);
//     }
//
//     printf("Producer %d: Finished producing %d items\n",
//            producer_id, ITEMS_PER_PRODUCER);
//     return NULL;
// }

// TODO: Implement consumer thread function
// void* consumer(void* arg) {
//     int consumer_id = (int)(long)arg;
//
//     while (1) {
//         // TODO: Lock mutex
//         // pthread_mutex_lock(&mutex);
//
//         // TODO: Wait while buffer is empty AND not all items consumed
//         // while (count == 0 && items_consumed < TOTAL_ITEMS) {
//         //     printf("Consumer %d: Buffer empty, waiting...\n", consumer_id);
//         //     pthread_cond_wait(&not_empty, &mutex);
//         // }
//
//         // TODO: Check if done consuming
//         // if (items_consumed >= TOTAL_ITEMS) {
//         //     pthread_mutex_unlock(&mutex);
//         //     break;
//         // }
//
//         // TODO: Consume item (remove from buffer)
//         // int item = buffer[out_index];
//         // out_index = (out_index + 1) % BUFFER_SIZE;
//         // count--;
//         // items_consumed++;
//         // printf("Consumer %d: Consumed item %d, buffer count: %d\n",
//         //        consumer_id, item, count);
//
//         // TODO: Signal that buffer is not full
//         // pthread_cond_signal(&not_full);
//
//         // TODO: Unlock mutex
//         // pthread_mutex_unlock(&mutex);
//
//         // TODO: Simulate consumption time
//         // usleep(rand() % 15000);
//     }
//
//     printf("Consumer %d: Finished consuming\n", consumer_id);
//     return NULL;
// }

int main() {
    // TODO: Declare thread arrays
    // pthread_t producers[NUM_PRODUCERS];
    // pthread_t consumers[NUM_CONSUMERS];

    printf("Starting Producer-Consumer demonstration...\n");
    // TODO: Uncomment after defining constants
    // printf("Buffer capacity: %d\n", BUFFER_SIZE);
    // printf("Producers: %d (%d items each)\n", NUM_PRODUCERS, ITEMS_PER_PRODUCER);
    // printf("Consumers: %d\n", NUM_CONSUMERS);
    // printf("Total items: %d\n\n", TOTAL_ITEMS);

    // TODO: Create producer threads
    // for (int i = 0; i < NUM_PRODUCERS; i++) {
    //     pthread_create(&producers[i], NULL, producer, (void*)(long)i);
    // }

    // TODO: Create consumer threads
    // for (int i = 0; i < NUM_CONSUMERS; i++) {
    //     pthread_create(&consumers[i], NULL, consumer, (void*)(long)i);
    // }

    // TODO: Wait for all producers
    // for (int i = 0; i < NUM_PRODUCERS; i++) {
    //     pthread_join(producers[i], NULL);
    // }

    // TODO: Signal consumers that production is done
    // pthread_cond_broadcast(&not_empty);

    // TODO: Wait for all consumers
    // for (int i = 0; i < NUM_CONSUMERS; i++) {
    //     pthread_join(consumers[i], NULL);
    // }

    // TODO: Print final results
    // printf("\n===== RESULTS =====\n");
    // printf("Total items produced: %d\n", TOTAL_ITEMS);
    // printf("Total items consumed: %d\n", items_consumed);
    // if (items_consumed == TOTAL_ITEMS) {
    //     printf("SUCCESS: All items accounted for!\n");
    // } else {
    //     printf("ERROR: Item count mismatch!\n");
    // }

    // TODO: Cleanup
    // pthread_mutex_destroy(&mutex);
    // pthread_cond_destroy(&not_full);
    // pthread_cond_destroy(&not_empty);

    return 0;
}
