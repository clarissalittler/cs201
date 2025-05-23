#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

/*
 * Global counter variable shared by all threads
 */
long counter = 0;

/*
 * Semaphore for protecting access to the counter
 * 
 * TEACHING POINT: This program is functionally identical to mutSem.c.
 * Both demonstrate using a binary semaphore (initial value 1) as a mutex
 * to protect a shared counter from race conditions.
 */
sem_t counterSem;

/*
 * Thread function that safely increments the counter 100,000 times
 * 
 * TEACHING POINT: This demonstrates the standard pattern for using
 * a semaphore to protect a critical section:
 * 1. sem_wait: Decrement the semaphore count, blocking if it's already 0
 * 2. Access the protected resource
 * 3. sem_post: Increment the semaphore count, allowing another thread to proceed
 */
void* inc(void* arg){
  for(int i = 0; i < 100000; i++){
    /*
     * Wait on the semaphore before accessing the counter
     * This decrements the semaphore value, blocking if it's already 0
     */
    sem_wait(&counterSem);
    
    /*
     * Critical section - only one thread can execute this at a time
     */
    counter++;
    
    /*
     * Post to the semaphore after modifying the counter
     * This increments the semaphore value, allowing another thread to proceed
     */
    sem_post(&counterSem);
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
       */
      sem_init(&counterSem,0,1);
      
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
       * TEACHING POINT: The comment incorrectly calls this a "mutex" instead of
       * a semaphore, illustrating how binary semaphores and mutexes are conceptually
       * similar and sometimes conflated.
       */
      sem_destroy(&counterSem);

      /*
       * Print the final counter value
       * 
       * TEACHING POINT: With proper synchronization via the semaphore,
       * the final value should be exactly 100,000,000 (1000 threads * 100,000 increments),
       * demonstrating that the semaphore effectively prevented race conditions.
       */
      printf("Final counter value: %ld (Expected: %d)\n", counter, 1000 * 100000);
      pthread_exit(NULL);
}