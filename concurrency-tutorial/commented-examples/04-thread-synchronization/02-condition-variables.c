/**
 * ============================================================================
 * Condition Variables - Thread Signaling and Waiting
 * ============================================================================
 * 
 * This example demonstrates how to use condition variables to coordinate
 * between threads. Condition variables allow threads to:
 * - Wait until a specific condition becomes true
 * - Signal other threads when a condition changes
 * 
 * We'll implement a classic producer-consumer pattern with a bounded buffer
 * to illustrate condition variable usage.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* Size of our bounded buffer */
#define BUFFER_SIZE 5

/* Number of items each producer and consumer will process */
#define ITEMS_PER_PRODUCER 10
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

/* Define MAX macro for calculating max value between two numbers */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Bounded buffer shared between producer and consumer threads */
typedef struct {
    int buffer[BUFFER_SIZE];  /* The actual data buffer */
    int in;                   /* Index for next insertion */
    int out;                  /* Index for next removal */
    int count;                /* Number of items currently in buffer */
    pthread_mutex_t mutex;    /* Mutex to protect buffer access */
    pthread_cond_t not_full;  /* Condition: buffer is not full */
    pthread_cond_t not_empty; /* Condition: buffer is not empty */
} BoundedBuffer;

/* Global bounded buffer */
BoundedBuffer buffer;

/* Thread function prototypes */
void* producer(void* arg);
void* consumer(void* arg);

int main() {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int ids[MAX(NUM_PRODUCERS, NUM_CONSUMERS)];
    int i, rc;
    
    printf("Condition Variables Demonstration: Producer-Consumer Problem\n\n");
    
    /* Initialize buffer state */
    buffer.in = 0;
    buffer.out = 0;
    buffer.count = 0;
    
    /* Initialize synchronization primitives */
    pthread_mutex_init(&buffer.mutex, NULL);
    pthread_cond_init(&buffer.not_full, NULL);
    pthread_cond_init(&buffer.not_empty, NULL);
    
    printf("Creating %d producer and %d consumer threads...\n", 
           NUM_PRODUCERS, NUM_CONSUMERS);
    
    /* Create the producer threads */
    for (i = 0; i < NUM_PRODUCERS; i++) {
        ids[i] = i + 1;  /* Producer IDs start at 1 */
        rc = pthread_create(&producers[i], NULL, producer, &ids[i]);
        if (rc != 0) {
            perror("pthread_create (producer)");
            return EXIT_FAILURE;
        }
    }
    
    /* Create the consumer threads */
    for (i = 0; i < NUM_CONSUMERS; i++) {
        ids[i] = i + 1;  /* Consumer IDs start at 1 */
        rc = pthread_create(&consumers[i], NULL, consumer, &ids[i]);
        if (rc != 0) {
            perror("pthread_create (consumer)");
            return EXIT_FAILURE;
        }
    }
    
    /* Wait for all producer threads to complete */
    for (i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
        printf("Producer %d has completed.\n", i + 1);
    }
    
    /* Wait for all consumer threads to complete */
    for (i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
        printf("Consumer %d has completed.\n", i + 1);
    }
    
    /* Clean up synchronization primitives */
    pthread_mutex_destroy(&buffer.mutex);
    pthread_cond_destroy(&buffer.not_full);
    pthread_cond_destroy(&buffer.not_empty);
    
    printf("\nAll threads have completed. Demonstration finished.\n");
    
    return EXIT_SUCCESS;
}

/**
 * Insert an item into the bounded buffer.
 * 
 * This function will block if the buffer is full, waiting until space is available.
 * 
 * @param id The producer ID (for logging)
 * @param item The item to insert
 */
void insert_item(int id, int item) {
    /* Acquire the mutex lock to protect buffer access */
    pthread_mutex_lock(&buffer.mutex);
    
    /* 
     * Wait while the buffer is full.
     * pthread_cond_wait atomically releases the mutex and blocks on the condition.
     * When signaled, it reacquires the mutex before returning.
     */
    while (buffer.count == BUFFER_SIZE) {
        printf("Producer %d: Buffer FULL. Waiting...\n", id);
        pthread_cond_wait(&buffer.not_full, &buffer.mutex);
    }
    
    /* At this point, buffer is not full and we have the mutex */
    
    /* Insert the item into the buffer */
    buffer.buffer[buffer.in] = item;
    
    /* Update buffer state */
    buffer.in = (buffer.in + 1) % BUFFER_SIZE;
    buffer.count++;
    
    printf("Producer %d: Inserted item %d (count=%d)\n", id, item, buffer.count);
    
    /* Signal that the buffer is no longer empty */
    pthread_cond_signal(&buffer.not_empty);
    
    /* Release the mutex lock */
    pthread_mutex_unlock(&buffer.mutex);
}

/**
 * Remove an item from the bounded buffer.
 * 
 * This function will block if the buffer is empty, waiting until an item is available.
 * 
 * @param id The consumer ID (for logging)
 * @return The item removed from the buffer
 */
int remove_item(int id) {
    int item;
    
    /* Acquire the mutex lock to protect buffer access */
    pthread_mutex_lock(&buffer.mutex);
    
    /* 
     * Wait while the buffer is empty
     * Similar to the producer, this atomically releases the mutex and blocks
     */
    while (buffer.count == 0) {
        printf("Consumer %d: Buffer EMPTY. Waiting...\n", id);
        pthread_cond_wait(&buffer.not_empty, &buffer.mutex);
    }
    
    /* At this point, buffer is not empty and we have the mutex */
    
    /* Remove the item from the buffer */
    item = buffer.buffer[buffer.out];
    
    /* Update buffer state */
    buffer.out = (buffer.out + 1) % BUFFER_SIZE;
    buffer.count--;
    
    printf("Consumer %d: Removed item %d (count=%d)\n", id, item, buffer.count);
    
    /* Signal that the buffer is no longer full */
    pthread_cond_signal(&buffer.not_full);
    
    /* Release the mutex lock */
    pthread_mutex_unlock(&buffer.mutex);
    
    return item;
}

/**
 * Producer thread function.
 * 
 * Produces ITEMS_PER_PRODUCER items and inserts them into the buffer.
 * 
 * @param arg Pointer to the producer's ID
 * @return Always NULL
 */
void* producer(void* arg) {
    int id = *((int*)arg);
    int i;
    
    printf("Producer %d: Starting...\n", id);
    
    /* Produce items and insert them into the buffer */
    for (i = 0; i < ITEMS_PER_PRODUCER; i++) {
        /* Create an item - here we use a simple integer based on producer ID and count */
        int item = id * 100 + i;
        
        /* Simulate some work being done to produce the item */
        usleep(rand() % 100000);  /* Sleep 0-100ms */
        
        /* Insert the item into the buffer */
        insert_item(id, item);
    }
    
    printf("Producer %d: Finished producing %d items.\n", id, ITEMS_PER_PRODUCER);
    
    return NULL;
}

/**
 * Consumer thread function.
 * 
 * Consumes ITEMS_PER_PRODUCER items from the buffer.
 * 
 * @param arg Pointer to the consumer's ID
 * @return Always NULL
 */
void* consumer(void* arg) {
    int id = *((int*)arg);
    int i, item;
    int total_consumed = 0;
    
    /* Calculate how many items this consumer should consume */
    int items_to_consume = (NUM_PRODUCERS * ITEMS_PER_PRODUCER) / NUM_CONSUMERS;
    
    /* Adjust for last consumer if division isn't even */
    if (id == NUM_CONSUMERS) {
        items_to_consume += (NUM_PRODUCERS * ITEMS_PER_PRODUCER) % NUM_CONSUMERS;
    }
    
    printf("Consumer %d: Starting to consume %d items...\n", id, items_to_consume);
    
    /* Consume items from the buffer */
    for (i = 0; i < items_to_consume; i++) {
        /* Remove an item from the buffer */
        item = remove_item(id);
        total_consumed++;
        
        /* Simulate processing the item */
        usleep(rand() % 150000);  /* Sleep 0-150ms (on average, consumers are slower) */
        
        /* Do something with the item (just print for this example) */
        printf("Consumer %d: Processed item %d\n", id, item);
    }
    
    printf("Consumer %d: Finished consuming %d items.\n", id, total_consumed);
    
    return NULL;
}

/**
 * EXPLANATION:
 * 
 * Condition Variables and Thread Coordination:
 * 
 * 1. What Are Condition Variables?
 *    - Synchronization primitive for thread signaling and waiting
 *    - Allow threads to wait for a specific condition to occur
 *    - Always used in conjunction with a mutex
 *    - Enable efficient waiting without busy-waiting (using CPU)
 * 
 * 2. Key Condition Variable Operations:
 *    - pthread_cond_init: Initialize a condition variable
 *    - pthread_cond_wait: Wait until the condition is signaled
 *    - pthread_cond_signal: Wake up one waiting thread
 *    - pthread_cond_broadcast: Wake up all waiting threads
 *    - pthread_cond_destroy: Clean up condition variable resources
 * 
 * 3. The pthread_cond_wait Operation:
 *    - MUST be called with the mutex locked
 *    - Atomically releases the mutex and blocks the thread
 *    - Upon returning (after signal), the mutex is locked again
 *    - Always use in a loop checking the actual condition (to handle spurious wakeups)
 * 
 * 4. Producer-Consumer Pattern:
 *    - Producers generate data and add it to a shared buffer
 *    - Consumers remove data from the buffer and process it
 *    - Bounded buffer has limited capacity, requiring coordination
 *    - Condition variables are used for:
 *      * not_full: Signals when space is available (wait condition for producers)
 *      * not_empty: Signals when items are available (wait condition for consumers)
 * 
 * 5. Why the While Loop in Wait?
 *    - Protects against spurious wakeups (when a thread wakes without a signal)
 *    - Handles the case where multiple threads are awakened but only one can proceed
 *    - Ensures the condition is still true after acquiring the mutex again
 *    - ALWAYS use "while (condition)" instead of "if (condition)" with pthread_cond_wait
 * 
 * 6. Deadlock Prevention:
 *    - Always check for proper mutex acquisition and release
 *    - Ensure signals are sent when conditions change
 *    - Be careful about the order of acquiring multiple locks
 *    - Consider using timeouts (pthread_cond_timedwait) for robustness
 * 
 * 7. Common Patterns Using Condition Variables:
 *    - Producer-Consumer (shown here)
 *    - Reader-Writer (multiple readers, exclusive writers)
 *    - Barriers (wait for all threads to reach a point)
 *    - Thread pools
 *    - Delayed initialization
 *    - Event notification systems
 */