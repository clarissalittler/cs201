#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/*
 * Maximum number of threads that can be in the critical section simultaneously
 * 
 * TEACHING POINT: Unlike a mutex which allows only one thread at a time,
 * this example uses a semaphore to allow multiple threads (3) to access
 * the critical section concurrently. This demonstrates the counting nature
 * of semaphores, which can allow controlled concurrent access.
 */
const int maxConcurrent = 3;

/*
 * Total number of threads to create
 */
const int totalThreads = 50;

/*
 * Semaphore to control access to the shared resource
 */
sem_t sem;

/*
 * Thread function that simulates handling a connection
 * 
 * TEACHING POINT: This demonstrates how semaphores can be used to
 * implement resource pooling or rate limiting. Each thread represents
 * a task that needs access to a limited resource (e.g., a connection pool).
 */
void* handler(void* arg) {
    /*
     * Extract the thread ID from the argument
     */
    int id = *(int*)arg;
    
    /*
     * Wait on the semaphore to acquire access
     * 
     * TEACHING POINT: If there are already maxConcurrent threads in the
     * critical section, this thread will block until one of them completes
     * and calls sem_post. This implements a form of admission control.
     */
    sem_wait(&sem); // Acquire semaphore

    /*
     * Critical section begins - this thread has acquired access to the resource
     * 
     * TEACHING POINT: Unlike mutex examples where only one thread can be in
     * the critical section at a time, this semaphore allows up to maxConcurrent (3)
     * threads to be in this section simultaneously.
     */
    printf("Thread %d handling connection.\n", id);
    
    /*
     * Simulate doing some work for 1-4 seconds
     */
    sleep(rand()%4 + 1); // Simulate work
    
    /*
     * Done with the work
     */
    printf("Thread %d done.\n", id);

    /*
     * Release the semaphore, allowing another waiting thread to proceed
     * 
     * TEACHING POINT: This increments the semaphore count, allowing another
     * waiting thread to enter the critical section. This ensures that at most
     * maxConcurrent threads are in the critical section at any time.
     */
    sem_post(&sem); // Increments semaphore
    return NULL;
}

int main() {
  /*
   * Initialize random number generator for the work simulation
   */
  srand(time(0));
  
  /*
   * Array to hold thread identifiers
   */
  pthread_t threads[totalThreads];
  
  /*
   * Array to store thread IDs to pass to threads
   */
  int ids[totalThreads];

  /*
   * Initialize the semaphore with value maxConcurrent (3)
   * 
   * TEACHING POINT: This is a key difference from the previous examples.
   * By initializing the semaphore to 3 instead of 1, we allow up to 3 threads
   * to be in the critical section simultaneously. This demonstrates how
   * semaphores can be used for resource pooling and concurrency control.
   */
  sem_init(&sem, 0, maxConcurrent); 

  /*
   * Create threads
   * 
   * TEACHING POINT: We create 50 threads but only allow 3 to run concurrently.
   * This simulates a server handling many connections with a limited thread pool.
   */
  for (int i = 0; i < totalThreads; i++) {
    ids[i] = i + 1;
    pthread_create(&threads[i], NULL, handler, &ids[i]);
  }

  /*
   * Wait for all threads to complete
   * 
   * TEACHING POINT: Unlike the philosopher examples which run forever,
   * this example will eventually complete because each thread does a finite
   * amount of work and then terminates.
   */
  for (int i = 0; i < totalThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  /*
   * Clean up the semaphore
   */
  sem_destroy(&sem); 
  return 0;
}