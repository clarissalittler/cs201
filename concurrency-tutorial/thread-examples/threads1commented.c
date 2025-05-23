#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

/*
 * Thread function that sleeps for a random duration
 * 
 * TEACHING POINT: This is a simple thread function that demonstrates:
 * 1. Threads execute independently with their own flow of control
 * 2. Threads can perform time-consuming operations without blocking the main thread
 * 3. Thread functions must follow the void* func(void*) signature pattern
 */
void* myFun(void* arg){
  /* 
   * Generate a random sleep duration between 1 and 3 seconds
   */
  int temp = rand()%3+1;
  
  /* 
   * Sleep for the random duration
   * 
   * TEACHING POINT: Each thread will sleep for a different amount of time,
   * demonstrating that threads execute independently and concurrently.
   */
  sleep(temp);
  
  /* 
   * Print the sleep duration
   * 
   * TEACHING POINT: Thread output is interleaved in the terminal.
   * The order of these print statements will vary between runs
   * due to the randomized sleep durations and thread scheduling.
   */
  printf("I slept for %d seconds\n",temp);

  /* 
   * Return NULL since this thread doesn't produce a result
   * 
   * TEACHING POINT: Thread functions must return something, even if
   * it's just NULL. Returning NULL indicates the thread completed successfully
   * but has no specific result to report.
   */
  return NULL;
}

int main(){
  /* 
   * Initialize random number generator
   */
  srand(time(0));
  
  /* 
   * Define variables to hold thread identifiers
   */
  pthread_t thread1;
  pthread_t thread2;

  /* 
   * Create the first thread
   * 
   * TEACHING POINT: The pthread_create function has this signature:
   * int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
   *                    void *(*start_routine) (void *), void *arg);
   * 
   * The function's signature is commented below on line 24, showing that:
   * - First parameter is a pointer to a pthread_t to store the thread ID
   * - Second is thread attributes (NULL for defaults)
   * - Third is a function pointer to a function that returns void* and takes void*
   * - Fourth is the argument to pass to the thread function
   */
  pthread_create(&thread1,NULL,myFun,NULL);
  // void pthread_create (blah,blah,(void*)(*threadFunction)(void*),void*)
  
  /* 
   * Create the second thread
   */
  pthread_create(&thread2,NULL,myFun,NULL);

  /* 
   * Wait for threads to complete
   * 
   * TEACHING POINT: pthread_join waits for a thread to terminate.
   * This is a synchronization point between the main thread and
   * the child threads. The main thread will block here until both
   * child threads have completed.
   * 
   * The function's signature is informally commented on line 28:
   * - First parameter is the thread ID to wait for
   * - Second is a pointer to store the thread's return value
   *   (NULL means we don't care about the return value)
   */
  pthread_join(thread1,NULL); // pthread_t,void**
  pthread_join(thread2,NULL); // 
  
  return 0;
}