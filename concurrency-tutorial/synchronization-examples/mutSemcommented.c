#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

/*
 * Global counter variable shared by all threads
 */
long counter = 0;

/*
 * Semaphore used for synchronization
 * 
 * TEACHING POINT: This example demonstrates using a semaphore as a mutex.
 * A semaphore is a more general synchronization primitive than a mutex.
 * When initialized with a value of 1 (as it is in this example), a semaphore
 * can function as a mutex, providing mutual exclusion.
 * 
 * The key difference between semaphores and mutexes is that semaphores:
 * - Maintain a count (not just locked/unlocked state)
 * - Can be used for signaling between threads (not just mutual exclusion)
 * - Can allow multiple threads to access a resource simultaneously (when count > 1)
 */
sem_t counterMutex;

/*
 * Thread function that safely increments the counter 100,000 times
 * 
 * TEACHING POINT: This demonstrates the pattern for using a semaphore
 * as a mutex to protect a critical section:
 * 1. Wait on the semaphore (sem_wait) - decrements the count, blocks if count is 0
 * 2. Access the shared resource
 * 3. Post to the semaphore (sem_post) - increments the count, unblocks waiting threads
 */
void* inc(void* arg){
  for(int i = 0; i < 100000; i++){
    /*
     * Wait on the semaphore before accessing the shared counter
     * This decrements the semaphore count. If the count is already 0,
     * the thread will block until another thread calls sem_post.
     */
    sem_wait(&counterMutex);
    
    /*
     * Critical section - only one thread can execute this at a time
     * (because the semaphore was initialized with a count of 1)
     */
    counter++;
    
    /*
     * Post to the semaphore after accessing the counter
     * This increments the semaphore count, allowing another thread to proceed.
     */
    sem_post(&counterMutex);
  }
  return NULL;
}

int main() {
    /*
     * Array to hold thread identifiers for 1000 threads
     */
    pthread_t threads[1000];

    /*
     * Initialize the semaphore with:
     * - First parameter (0): Shared between threads in the same process
     * - Second parameter (1): Initial value - allowing only one thread at a time
     * 
     * TEACHING POINT: A semaphore with initial value 1 acts like a mutex,
     * allowing only one thread at a time to enter the critical section.
     * When used this way, it's often called a "binary semaphore."
     */
    sem_init(&counterMutex, 0, 1); // init value
    
    /*
     * Create 1000 threads, each running the inc function
     */
    for (long t = 0; t < 1000; t++) {
        pthread_create(&threads[t], NULL, inc, NULL);
    }
    
    /*
     * Wait for all threads to complete
     */
    for (int t = 0; t < 1000; t++) {
        pthread_join(threads[t], NULL);
    }

    /*
     * Clean up the semaphore
     * 
     * TEACHING POINT: Although this comment calls it a "mutex", it's actually
     * destroying a semaphore. This reflects how semaphores with an initial count
     * of 1 are commonly thought of as mutex-like in their behavior.
     */
    sem_destroy(&counterMutex);

    /*
     * Print the final counter value
     * 
     * TEACHING POINT: The result should be identical to the mut1.c example.
     * This demonstrates that a binary semaphore can be used interchangeably with
     * a mutex for simple mutual exclusion scenarios.
     */
    printf("Final counter value: %ld (Expected: %d)\n", counter, 1000 * 100000);
    pthread_exit(NULL);
}