#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
 * Global counter variable shared by all threads
 * 
 * TEACHING POINT: This shared variable demonstrates the need for
 * synchronization in multi-threaded programs. Without protection,
 * concurrent updates would lead to race conditions.
 */
int ourCounter = 0;

/*
 * Mutex to protect access to the counter
 * 
 * TEACHING POINT: This mutex will ensure that only one thread at a time
 * can access the counter, preventing race conditions.
 */
pthread_mutex_t mutex;


/*
 * Thread function that safely increments the counter
 * 
 * TEACHING POINT: This demonstrates the standard pattern for using
 * a mutex to protect a critical section:
 * 1. Lock the mutex (pthread_mutex_lock)
 * 2. Access the shared resource
 * 3. Unlock the mutex (pthread_mutex_unlock)
 */
void* threadCounter(void* arg){
  /*
   * Lock the mutex before accessing the shared counter
   * If another thread already holds the lock, this call will
   * block until the mutex becomes available.
   */
  pthread_mutex_lock(&mutex);
  
  /*
   * Critical section begins - only one thread can execute this at a time
   */
  int temp = ourCounter;
  
  /*
   * Introduce a random delay to simulate work and make the race condition
   * more likely to occur if synchronization weren't used
   */
  sleep(rand()%3);
  
  /*
   * Complete the counter update
   * 
   * TEACHING POINT: Without the mutex, this operation would be subject to
   * race conditions because threads might overwrite each other's updates.
   */
  ourCounter = temp+1;
  
  /*
   * Release the mutex to allow other threads to access the counter
   */
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(){
  /*
   * Initialize random number generator
   */
  srand(time(0));
  
  /*
   * Array to hold thread identifiers for 10 threads
   */
  pthread_t threads[10];

  /*
   * Initialize the mutex with default attributes
   * 
   * TEACHING POINT: The comment notes that the second argument is for
   * configuration, but here we're using NULL for default configuration.
   * Mutex attributes could be used to specify properties like:
   * - Whether the mutex can be shared between processes
   * - Whether the mutex should use recursive locking
   * - How the mutex should handle priority inheritance
   */
  // the second argument is for configuring it
  pthread_mutex_init(&mutex,NULL);

  
  /*
   * Create 10 threads, each running the threadCounter function
   */
  for(int i=0;i<10;i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  /*
   * Wait for all threads to complete
   */
  for(int i=0; i< 10; i++){
    pthread_join(threads[i],NULL);
  }

  /*
   * Clean up the mutex when it's no longer needed
   */
  pthread_mutex_destroy(&mutex);

  /*
   * Print the final counter value
   * 
   * TEACHING POINT: With proper synchronization via the mutex,
   * the final value should always be 10 (one increment per thread).
   * This demonstrates that the mutex effectively prevented race conditions.
   */
  printf("What's the value of this counter? %d\n",ourCounter);
  return 0;
}