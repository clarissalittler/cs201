#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

/* 
 * Global variable shared by all threads
 * 
 * TEACHING POINT: All threads in a process share the same memory space,
 * so this variable is accessible to all threads. This allows threads
 * to communicate through shared memory, but also creates potential
 * for race conditions.
 */
int ourCounter = 0;

/* 
 * Global semaphore for thread synchronization
 * 
 * TEACHING POINT: Semaphores are synchronization primitives that
 * can be used to protect shared resources. They maintain a count
 * that threads can increment (post) or decrement (wait).
 */
sem_t ourSem;

/*
 * Thread function that safely increments the counter
 * 
 * TEACHING POINT: This function demonstrates the proper use of semaphores
 * to protect a critical section of code and prevent race conditions.
 */
void* threadCounter(void* arg){
  /* 
   * Wait on the semaphore (decrement its value)
   * If the semaphore value is 0, the thread will block until
   * another thread calls sem_post to increment it.
   * 
   * This ensures only one thread can enter the critical section at a time.
   */
  sem_wait(&ourSem);
  
  /* 
   * Critical section begins:
   * This code is protected by the semaphore, so only one thread
   * can execute it at a time.
   */
  int temp = ourCounter;
  
  /* 
   * Introduce a random delay to simulate work and increase
   * the chance of exposing race conditions (if they exist)
   */
  sleep(rand()%3);
  
  /* 
   * Complete the counter increment operation
   * The semaphore ensures this happens atomically across all threads.
   */
  ourCounter = temp+1;
  
  /* 
   * Signal the semaphore (increment its value)
   * This allows another waiting thread to enter the critical section.
   */
  sem_post(&ourSem);
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
   * Initialize the semaphore with:
   * - Sharing level: 0 (shared between threads in the same process)
   * - Initial value: 1 (allowing one thread to enter the critical section)
   * 
   * TEACHING POINT: A semaphore with initial value 1 is often called a "binary semaphore"
   * or "mutex semaphore" because it allows only one thread at a time to access the
   * protected resource. This is similar to a mutex but with different semantics.
   */
  sem_init(&ourSem,0,1);

  /* 
   * Create 10 threads that will all try to increment the counter
   */
  for(int i = 0;i < 10; i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  /* 
   * Wait for all threads to complete
   */
  for(int i =0; i < 10;i++){
    pthread_join(threads[i],NULL);
  }

  /* 
   * Clean up the semaphore after all threads are done
   * 
   * TEACHING POINT: Always destroy synchronization primitives
   * when they're no longer needed to avoid resource leaks.
   */
  sem_destroy(&ourSem);
  
  /* 
   * Print the final counter value
   * 
   * TEACHING POINT: Because we properly synchronized access to the counter 
   * with a semaphore, the final value should always be 10 (each of the 10
   * threads successfully incremented it once).
   * 
   * Without the semaphore (as in threads2.c), race conditions would likely
   * cause the final value to be less than 10.
   */
  printf("What's the value of this counter?? %d\n",ourCounter);
  return 0;
}