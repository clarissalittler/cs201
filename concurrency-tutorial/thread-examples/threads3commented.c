#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/*
 * Global counter variable shared by all threads
 */
int ourCounter = 0;

/*
 * Mutex for protecting access to the counter
 * 
 * TEACHING POINT: A mutex provides mutual exclusion - it ensures that
 * only one thread can access the protected resource at a time.
 * This prevents race conditions in shared data access.
 */
pthread_mutex_t mutex;

/*
 * Thread function that safely increments the counter by 1
 * 
 * TEACHING POINT: This demonstrates the proper pattern for accessing
 * shared data in a multi-threaded environment:
 * 1. Lock the mutex before accessing the shared resource
 * 2. Perform operations on the shared resource
 * 3. Unlock the mutex when done
 */
void* threadCounter(void* arg){
  /*
   * Lock the mutex to get exclusive access to the counter
   * If another thread already holds the lock, this thread will
   * block until the mutex becomes available.
   */
  pthread_mutex_lock(&mutex);
  
  /*
   * Critical section begins - only one thread can execute this at a time
   */
  int temp = ourCounter;

  /*
   * Simulate work with a random delay
   * Unlike threads2.c, this delay doesn't cause a race condition
   * because the mutex protects the entire operation.
   */
  sleep(rand()%3);
  
  /*
   * Update the counter
   */
  ourCounter = temp +1;
  
  /*
   * Release the mutex to allow other threads to access the counter
   * 
   * TEACHING POINT: Always ensure mutex unlocking happens, even when
   * exceptions or errors occur. In more complex code, you'd typically
   * use try/finally blocks or RAII patterns to guarantee this.
   */
  pthread_mutex_unlock(&mutex);
  return NULL;
}

/*
 * Alternative thread function that increments the counter by a user-specified amount
 * 
 * TEACHING POINT: This shows how different thread functions can safely
 * access the same shared resource when proper synchronization is used.
 */
void* wildCounter(void* arg){
  /*
   * Get user input for the increment amount
   * 
   * TEACHING POINT: This is outside the critical section because it
   * doesn't involve the shared resource. It's good practice to minimize
   * the code protected by a mutex to improve concurrency.
   */
  printf("Enter a number, right quick: ");
  int inc;
  scanf("%d",&inc);
  
  /*
   * Lock the mutex to get exclusive access to the counter
   */
  pthread_mutex_lock(&mutex);
  
  /*
   * Critical section begins
   */
  int temp = ourCounter;

  /*
   * Simulate work with a random delay
   */
  sleep(rand()%3);
  
  /*
   * Update counter with user-specified increment
   * 
   * TEACHING POINT: This thread function behaves differently by adding
   * a user-specified value instead of just 1, yet it can coexist with
   * the other thread function because both properly synchronize access.
   */
  ourCounter = temp + inc;
  
  /*
   * Release the mutex
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
   * Array to hold 10 thread identifiers
   */
  pthread_t threads[10];

  /*
   * Initialize the mutex with default attributes
   * 
   * TEACHING POINT: Always initialize synchronization primitives
   * before using them. pthread_mutex_init is the proper way to
   * initialize a mutex.
   */
  pthread_mutex_init(&mutex,NULL);
  
  /*
   * Create 10 threads:
   * - 9 threads run the standard threadCounter function
   * - 1 thread (index 5) runs the wildCounter function
   * 
   * TEACHING POINT: This demonstrates how different thread functions
   * can coexist and safely interact with the same shared resources
   * when proper synchronization is used.
   */
  for(int i = 0; i < 10; i++){
    if(i != 5){
      pthread_create(&threads[i],NULL,threadCounter,NULL);
    }
    else{
      pthread_create(&threads[i],NULL,wildCounter,NULL);
    }
  }

  /*
   * Wait for all threads to complete
   */
  for(int i = 0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  /*
   * Clean up the mutex when it's no longer needed
   * 
   * TEACHING POINT: Always destroy synchronization primitives
   * when they're no longer needed to avoid resource leaks.
   */
  pthread_mutex_destroy(&mutex);
  
  /*
   * Print the final counter value
   * 
   * TEACHING POINT: With proper synchronization, the counter value will be
   * 9 + the number entered by the user. This demonstrates that all increments
   * were applied correctly without race conditions.
   */
  printf("What's the value of the counter? %d\n",ourCounter);
  
  return 0;
}