#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

/*
 * Global counter variable shared by all threads
 * 
 * TEACHING POINT: This shared variable will be accessed concurrently
 * by 1000 threads. Without synchronization, race conditions would occur
 * and the final value would be unpredictable. This example demonstrates
 * how to properly protect a shared resource using a mutex.
 */
long counter = 0;

/*
 * Mutex to protect access to the counter
 * 
 * TEACHING POINT: A mutex (mutual exclusion) is a synchronization primitive
 * that prevents multiple threads from accessing the same resource simultaneously.
 * Only one thread can hold the mutex at a time.
 */
pthread_mutex_t counterMutex;

/*
 * Thread function that safely increments the counter 100,000 times
 * 
 * TEACHING POINT: This function demonstrates the classic pattern for
 * using a mutex to protect a critical section:
 * 1. Lock the mutex (pthread_mutex_lock)
 * 2. Access the shared resource
 * 3. Unlock the mutex (pthread_mutex_unlock)
 */
void* inc(void* arg){
  for(int i = 0; i < 100000; i++){
    /*
     * Acquire the lock before accessing the shared counter
     * If another thread already holds the lock, this call will
     * block until the mutex becomes available.
     */
    pthread_mutex_lock(&counterMutex);
    
    /*
     * Critical section - only one thread can execute this at a time
     * This simple increment operation might look atomic, but on many
     * architectures it involves multiple machine instructions (read,
     * modify, write) which can be interleaved without protection.
     */
    counter++;
    
    /*
     * Release the lock to allow other threads to access the counter
     * This is crucial - failing to unlock would cause a deadlock.
     */
    pthread_mutex_unlock(&counterMutex);
  }
  return NULL;
}

int main() {
    /*
     * Array to hold thread identifiers for 1000 threads
     * 
     * TEACHING POINT: This demonstrates a high-concurrency scenario
     * with 1000 threads all trying to update the same variable.
     */
    pthread_t threads[1000];

    /*
     * Initialize the mutex with default attributes
     * Must be done before any threads try to use it
     * 
     * TEACHING POINT: Always initialize synchronization primitives
     * before they are used. Failing to do so results in undefined behavior.
     */
    pthread_mutex_init(&counterMutex, NULL);
    
    /*
     * Create 1000 threads, each running the inc function
     * Each thread will increment the counter 100,000 times
     */
    for (long t = 0; t < 1000; t++) {
        pthread_create(&threads[t], NULL, inc, NULL);
    }
    
    /*
     * Wait for all threads to complete
     * This ensures we don't check the final counter value
     * until all threads have finished their increments.
     */
    for (int t = 0; t < 1000; t++) {
        pthread_join(threads[t], NULL);
    }

    /*
     * Clean up the mutex when it's no longer needed
     * 
     * TEACHING POINT: Always destroy synchronization primitives
     * when they're no longer needed to avoid resource leaks.
     */
    pthread_mutex_destroy(&counterMutex);

    /*
     * Print the final counter value
     * 
     * TEACHING POINT: With proper synchronization, the final value
     * should always be 100,000,000 (1000 threads * 100,000 increments).
     * Without the mutex, race conditions would result in a much lower value.
     */
    printf("Final counter value: %ld (Expected: %d)\n", counter, 1000 * 100000);
    pthread_exit(NULL);
}