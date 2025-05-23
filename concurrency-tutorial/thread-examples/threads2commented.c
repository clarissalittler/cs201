#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/*
 * Global counter variable shared by all threads
 * 
 * TEACHING POINT: This shared variable demonstrates the fundamental
 * challenge of multi-threaded programming. All threads can access and
 * modify this variable concurrently, leading to race conditions.
 */
int ourCounter = 0;

/*
 * Thread function that increments the counter
 * 
 * TEACHING POINT: This function contains a classic race condition!
 * The sequence of operations (read counter, delay, write counter)
 * is not atomic, so multiple threads can interfere with each other.
 */
void* threadCounter(void* arg){
  /*
   * Read the current counter value into a local variable
   * 
   * TEACHING POINT: This line and the update line below create the race condition.
   * The thread reads the counter, but before it updates it, another thread might
   * have already modified the value.
   */
  int temp = ourCounter;

  /*
   * Introduce a random delay to simulate work and make the race condition
   * more likely to occur
   * 
   * TEACHING POINT: This sleep dramatically increases the chances of the
   * race condition occurring. Without it, the operations might happen so
   * quickly that you'd seldom observe the race condition.
   */
  sleep(rand()%3);
  
  /*
   * Update the counter by incrementing the local value
   * 
   * TEACHING POINT: This update might overwrite changes made by other threads
   * that ran during the sleep period. For example:
   * 1. Thread A reads counter = 0, then sleeps
   * 2. Thread B reads counter = 0, then sleeps
   * 3. Thread A wakes up, sets counter = 0+1 = 1
   * 4. Thread B wakes up, sets counter = 0+1 = 1 (overwriting Thread A's update)
   * Result: Counter = 1, even though two increments happened!
   */
  ourCounter = temp +1;
  return NULL;
}

int main(){
  /*
   * Initialize random number generator
   */
  srand(time(0));

  /*
   * Array to hold 10 thread identifiers
   * We'll create 10 threads that all try to increment the counter
   */
  pthread_t threads[10];

  /*
   * Create 10 threads, each running the threadCounter function
   * 
   * TEACHING POINT: All 10 threads operate on the same shared counter variable.
   * Without proper synchronization, they will likely encounter race conditions.
   */
  for(int i = 0; i < 10; i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  /*
   * Wait for all threads to complete
   */
  for(int i = 0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  /*
   * Print the final counter value
   * 
   * TEACHING POINT: Due to the race condition, the counter value will
   * usually be less than 10, even though 10 threads each tried to increment it once.
   * 
   * This demonstrates why synchronization is necessary when multiple threads
   * access shared data. Compare with thread2.c which uses a semaphore to
   * protect the counter.
   */
  printf("What's the value of the counter? %d\n",ourCounter);
  
  return 0;
}