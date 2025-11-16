// PEDAGOGICAL PURPOSE:
// This program demonstrates the classic producer-consumer problem using condition variables.
// It shows multiple producers and consumers sharing a bounded buffer with proper synchronization.
// Key learning objectives:
// 1. Understanding the producer-consumer pattern
// 2. Learning bounded buffer management with circular arrays
// 3. Understanding TWO condition variables (not full, not empty)
// 4. Seeing why separate conditions prevent unnecessary wakeups
// 5. Learning circular buffer indexing with modulo operator
// 6. Understanding multiple producers/consumers competing for resources
// 7. Seeing practical use of pthread_cond_wait/signal in complex scenarios
// 8. Learning proper resource management with multiple threads

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// CONFIGURATION CONSTANTS:
// These define the problem size
#define BUFFER_SIZE 5     // Maximum items in buffer (bounded buffer)
#define NUM_ITEMS 20      // Total items each producer will produce

// CIRCULAR BUFFER:
// Fixed-size array used as a circular queue
// Items added at 'in' index, removed at 'out' index
//
// WHY CIRCULAR?
// Reuses array space efficiently
// When we reach end, wrap around to beginning
int buffer[BUFFER_SIZE];

// BUFFER STATE VARIABLES:
// These track the current state of the buffer
// All protected by mutex
int count = 0;      // Number of items currently in buffer (0 to BUFFER_SIZE)
int in = 0;         // Index where next item will be produced (insert position)
int out = 0;        // Index where next item will be consumed (remove position)

// SYNCHRONIZATION PRIMITIVES:
//
// ONE MUTEX:
// Protects all shared state (buffer, count, in, out)
pthread_mutex_t mutex;

// TWO CONDITION VARIABLES:
// Why two? Different conditions need different signals
//
// cond_not_empty:
// - Condition: buffer is not empty (count > 0)
// - Waited on by: consumers (need items to consume)
// - Signaled by: producers (after adding item)
//
// cond_not_full:
// - Condition: buffer is not full (count < BUFFER_SIZE)
// - Waited on by: producers (need space to add)
// - Signaled by: consumers (after removing item)
pthread_cond_t cond_not_empty;
pthread_cond_t cond_not_full;

// PRODUCER THREAD:
// Produces items and adds them to the buffer
// Waits if buffer is full
//
// REAL-WORLD ANALOGY:
// Factory worker adding boxes to a conveyor belt
// If belt is full, worker waits for space
void* producer(void* arg) {

    // GET PRODUCER ID:
    // For logging/debugging purposes
    // Helps us see which producer is doing what
    int id = *(int*)arg;

    // PRODUCE NUM_ITEMS ITEMS:
    // Each producer creates 20 items
    // With 2 producers, total of 40 items produced
    for (int i = 0; i < NUM_ITEMS; i++) {

        // SIMULATE PRODUCTION TIME:
        // In real application, this might be:
        //   - Reading from network
        //   - Computing result
        //   - Generating data
        sleep(1);

        // ENTER CRITICAL SECTION:
        // Must lock before accessing shared buffer
        pthread_mutex_lock(&mutex);

        // WAIT FOR SPACE IN BUFFER:
        // If buffer is full, wait until consumer makes space
        //
        // WHY WHILE LOOP?
        // - Spurious wakeups (thread wakes without signal)
        // - Multiple producers (another producer might fill space)
        //
        // SCENARIO:
        // 1. Buffer full, producer1 waits
        // 2. Consumer removes item, signals not_full
        // 3. Producer1 wakes up
        // 4. BUT producer2 might grab mutex first and fill buffer!
        // 5. Producer1 must recheck condition
        while (count == BUFFER_SIZE) {

            // WAIT FOR BUFFER TO HAVE SPACE:
            // Atomically: unlock mutex, sleep, wait for signal
            // When woken: relock mutex, return
            //
            // WHY THIS CONDITION VARIABLE?
            // We're waiting for buffer to NOT be full
            // Consumers signal this condition (they make space)
            pthread_cond_wait(&cond_not_full, &mutex);
        }
        // AT THIS POINT:
        // - Mutex is locked
        // - Buffer has space (count < BUFFER_SIZE)
        // - Safe to add item

        // PRODUCE AN ITEM:
        // In this simple example, item is just loop index
        // Could be any data in real application
        int item = i;

        // ADD ITEM TO BUFFER:
        // Insert at 'in' index
        buffer[in] = item;

        // ADVANCE INSERT INDEX (CIRCULAR):
        // in = (in + 1) % BUFFER_SIZE
        //
        // CIRCULAR BUFFER MAGIC:
        // BUFFER_SIZE = 5
        // If in = 4, next in = (4+1) % 5 = 0 (wrap around!)
        // If in = 2, next in = (2+1) % 5 = 3 (normal increment)
        //
        // This reuses the array space efficiently
        // Instead of needing infinite array, we reuse 5 slots
        in = (in + 1) % BUFFER_SIZE;

        // INCREMENT COUNT:
        // Track how many items in buffer
        count++;

        // LOG THE PRODUCTION:
        // Show which producer added which item
        // Show current buffer occupancy
        printf("Producer %d produced item %d (buffer count: %d)\n", id, item, count);

        // SIGNAL THAT BUFFER IS NOT EMPTY:
        // Wake up a consumer (if any are waiting)
        //
        // WHY THIS SIGNAL?
        // We just added an item
        // Buffer is definitely not empty now
        // Consumers waiting for items can proceed
        //
        // pthread_cond_signal vs pthread_cond_broadcast:
        // signal: wakes ONE consumer
        // broadcast: wakes ALL consumers
        // signal is sufficient (any one consumer can take the item)
        pthread_cond_signal(&cond_not_empty);

        // RELEASE MUTEX:
        // Done modifying shared state
        // Allow other threads to proceed
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

// CONSUMER THREAD:
// Consumes items from the buffer
// Waits if buffer is empty
//
// REAL-WORLD ANALOGY:
// Worker taking boxes from conveyor belt
// If belt is empty, worker waits for items
void* consumer(void* arg) {

    // GET CONSUMER ID:
    // For logging/debugging
    int id = *(int*)arg;

    // CONSUME NUM_ITEMS ITEMS:
    // Each consumer processes 20 items
    // With 2 consumers, total of 40 items consumed
    // Matches 40 items produced by 2 producers
    for (int i = 0; i < NUM_ITEMS; i++) {

        // ENTER CRITICAL SECTION:
        pthread_mutex_lock(&mutex);

        // WAIT FOR ITEMS IN BUFFER:
        // If buffer is empty, wait until producer adds item
        //
        // WHY WHILE LOOP?
        // Same reasons as producer:
        // - Spurious wakeups
        // - Multiple consumers (another might take the item)
        while (count == 0) {

            // WAIT FOR BUFFER TO HAVE ITEMS:
            // Producers signal this condition (they add items)
            pthread_cond_wait(&cond_not_empty, &mutex);
        }
        // AT THIS POINT:
        // - Mutex is locked
        // - Buffer has items (count > 0)
        // - Safe to remove item

        // CONSUME AN ITEM:
        // Remove from 'out' index
        int item = buffer[out];

        // ADVANCE REMOVE INDEX (CIRCULAR):
        // Same circular logic as producer's 'in' index
        out = (out + 1) % BUFFER_SIZE;

        // DECREMENT COUNT:
        // Track how many items in buffer
        count--;

        // LOG THE CONSUMPTION:
        // Show which consumer took which item
        printf("Consumer %d consumed item %d (buffer count: %d)\n", id, item, count);

        // SIGNAL THAT BUFFER IS NOT FULL:
        // Wake up a producer (if any are waiting)
        //
        // WHY THIS SIGNAL?
        // We just removed an item
        // Buffer definitely has space now
        // Producers waiting for space can proceed
        pthread_cond_signal(&cond_not_full);

        // RELEASE MUTEX:
        pthread_mutex_unlock(&mutex);

        // SIMULATE CONSUMPTION TIME:
        // Slower than production (2 seconds vs 1 second)
        // This creates interesting dynamics:
        //   - Producers will often wait (buffer fills up)
        //   - Demonstrates blocking behavior
        //
        // PLACEMENT NOTE:
        // Sleep is OUTSIDE critical section
        // Don't hold mutex while sleeping (wastes time)
        // Other threads can produce/consume while we sleep
        sleep(2);
    }

    return NULL;
}

int main() {

  // INITIALIZE SYNCHRONIZATION PRIMITIVES:
  // Must initialize before creating threads
  pthread_mutex_init(&mutex,NULL);
  pthread_cond_init(&cond_not_empty,NULL);
  pthread_cond_init(&cond_not_full,NULL);

  // THREAD HANDLES:
  // Two producers, two consumers
  pthread_t prod_thread1, cons_thread1, prod_thread2, cons_thread2;

  // THREAD IDs:
  // Each thread gets an ID for identification
  // Must be separate variables (each thread needs its own pointer)
  int prod_id1 = 1, cons_id1 = 1;
  int prod_id2 = 2, cons_id2 = 2;

  // CREATE THREADS:
  // Two producers and two consumers
  // All start running immediately
  //
  // COMPETITION:
  // Producers compete for buffer space
  // Consumers compete for buffer items
  // Mutex ensures no conflicts
  // Condition variables ensure efficient waiting
  pthread_create(&prod_thread1, NULL, producer, &prod_id1);
  pthread_create(&cons_thread1, NULL, consumer, &cons_id1);
  pthread_create(&prod_thread2, NULL, producer, &prod_id2);
  pthread_create(&cons_thread2, NULL, consumer, &cons_id2);

  // WAIT FOR PRODUCERS AND CONSUMERS:
  // Note: Only joining prod_thread1 and cons_thread1
  // This is likely a BUG in the original code!
  // Should also join prod_thread2 and cons_thread2
  //
  // CONSEQUENCE:
  // Program might exit before all threads finish
  // prod_thread2 and cons_thread2 might be killed
  // Not all items might be produced/consumed
  pthread_join(prod_thread1, NULL);
  pthread_join(cons_thread1, NULL);

  // MISSING:
  // pthread_join(prod_thread2, NULL);
  // pthread_join(cons_thread2, NULL);

  // CLEANUP:
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_not_empty);
  pthread_cond_destroy(&cond_not_full);

  return 0;
}

// EXPECTED OUTPUT (partial, order varies):
// Producer 1 produced item 0 (buffer count: 1)
// Producer 2 produced item 0 (buffer count: 2)
// Consumer 1 consumed item 0 (buffer count: 1)
// Producer 1 produced item 1 (buffer count: 2)
// Producer 2 produced item 1 (buffer count: 3)
// Consumer 2 consumed item 0 (buffer count: 2)
// ...
// (Order is non-deterministic due to scheduling)

// CIRCULAR BUFFER VISUALIZATION:
//
// INITIAL STATE:
// buffer: [_, _, _, _, _]  (empty)
// in: 0, out: 0, count: 0
//
// AFTER ADDING 3 ITEMS:
// buffer: [A, B, C, _, _]
// in: 3, out: 0, count: 3
//
// AFTER REMOVING 2 ITEMS:
// buffer: [A, B, C, _, _]  (A and B consumed, but still in memory)
// in: 3, out: 2, count: 1
//
// AFTER ADDING 4 MORE ITEMS (WRAPS AROUND):
// buffer: [E, F, C, D, E]
// in: 2, out: 2, count: 5 (FULL!)
// Items in order: C, D, E, F, E
//
// CIRCULAR PROPERTY:
// in and out chase each other around the array
// When they're equal: buffer is either empty or full (check count)

// WHY TWO CONDITION VARIABLES?
//
// COULD WE USE ONE?
// Yes, but inefficient:
//   pthread_cond_broadcast(&cond);  // Wake EVERYONE
//   All threads wake, most can't proceed
//   Wasted wakeups, wasted CPU
//
// WITH TWO:
// More precise signaling:
//   Producer adds item → signal cond_not_empty (wake consumer)
//   Consumer removes item → signal cond_not_full (wake producer)
//   Only threads that can proceed are woken
//   More efficient!

// RACE CONDITION SCENARIOS (if mutex was missing):
//
// SCENARIO 1: Two producers add simultaneously
//   Producer1: buffer[in] = item1; in++;
//   Producer2: buffer[in] = item2; in++;  (SAME in!)
//   Result: One item overwrites the other, count is wrong
//
// SCENARIO 2: Producer and consumer access simultaneously
//   Producer: buffer[2] = newItem; count++;
//   Consumer: item = buffer[2]; count--;  (at same time!)
//   Result: Undefined behavior, corrupted data
//
// MUTEX PREVENTS ALL OF THIS:
// Only one thread in critical section at a time

// KEY CONCEPTS:
//
// 1. PRODUCER-CONSUMER PATTERN:
//    Classic concurrency problem
//    Producers create data, consumers process it
//    Buffer decouples production rate from consumption rate
//
// 2. BOUNDED BUFFER:
//    Fixed-size buffer (prevents unlimited memory use)
//    Circular array for efficiency
//    Producers wait if full, consumers wait if empty
//
// 3. TWO CONDITION VARIABLES:
//    More efficient than one
//    Precise signaling: only wake threads that can proceed
//    cond_not_empty for consumers, cond_not_full for producers
//
// 4. CIRCULAR BUFFER INDEXING:
//    (index + 1) % SIZE wraps around
//    Reuses fixed array space
//    in and out pointers chase each other

// COMPILE AND RUN:
// gcc condtest.c -o condtest -pthread
// ./condtest
//
// Observe:
// - Items produced and consumed in non-deterministic order
// - Buffer count varies (0 to 5)
// - Threads block when buffer is full or empty
// - Note: Might not complete all items due to missing pthread_join calls

// EXPERIMENT:
// 1. Add the missing pthread_join calls for thread2
//    pthread_join(prod_thread2, NULL);
//    pthread_join(cons_thread2, NULL);
//    Now all items are produced/consumed
//
// 2. Change BUFFER_SIZE to 1
//    Observe more frequent blocking (buffer fills quickly)
//
// 3. Change consumer sleep to sleep(1) (same as producer)
//    More balanced, less blocking
//
// 4. Use only one condition variable
//    Change all signals to pthread_cond_broadcast(&single_cond)
//    Still works, but less efficient (more wakeups)
//
// 5. Add logging to see when threads wait/wake
//    printf("Producer %d waiting...\n", id);
//    (before pthread_cond_wait)
