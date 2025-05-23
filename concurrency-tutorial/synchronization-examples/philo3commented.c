#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

/*
 * Number of philosophers (and utensils) in the simulation
 */
#define numPhilosophers 5

/*
 * Array of mutexes, one for each utensil
 */
pthread_mutex_t utensils[numPhilosophers];

/*
 * Philosopher thread function
 * 
 * TEACHING POINT: This implementation uses a "resource hierarchy" solution
 * to the dining philosophers problem. By making at least one philosopher
 * (the last one) pick up utensils in a different order, it prevents the
 * circular wait condition necessary for deadlock.
 */
void* philosopher(void* num) {
    /*
     * Extract the philosopher's ID from the argument
     */
    int id = *(int*)num;
    
    /*
     * Variables to hold the utensil indices
     */
    int first;
    int sec;
    
    /*
     * The last philosopher (#4) picks up utensils in reverse order
     * 
     * TEACHING POINT: This is the key to preventing deadlock. By making the
     * last philosopher pick up utensils in reverse order, the circular dependency
     * chain is broken. Even if philosophers 0-3 each hold their first utensil,
     * philosopher 4 will pick up utensil 0 (breaking the circle) rather than utensil 4.
     */
    if(id != numPhilosophers-1){
      first = id;
      sec = id+1;
    }
    else {
      first = id+1;
      sec = id;
    }
    
    /*
     * Philosophers loop forever between thinking and eating
     */
    while (1) {
      printf("Philosopher %d is thinking.\n", id);
      sleep(1);
      
      /*
       * Pick up first utensil (which could be either left or right,
       * depending on the philosopher's ID)
       */
      pthread_mutex_lock(&utensils[first % numPhilosophers]);
      printf("Philosopher %d picked up utensil %d .\n", id, first % numPhilosophers);
      
      printf("Philosopher %d is thinking.\n", id);
      sleep(1);
      
      /*
       * Pick up second utensil
       */
      pthread_mutex_lock(&utensils[sec % numPhilosophers]);
       printf("Philosopher %d picked up utensil %d .\n", id, sec % numPhilosophers);
       
       /*
        * Eat for a moment now that both utensils are acquired
        */
       printf("Philosopher %d is eating.\n", id);
       sleep(1);
       
       /*
        * Put down utensils
        * 
        * TEACHING POINT: Interestingly, this implementation releases resources
        * in the order they were acquired rather than reverse order. This doesn't
        * affect correctness (since we're just releasing resources), but is different
        * from the conventional pattern seen in philo2.c.
        */
       pthread_mutex_unlock(&utensils[first % numPhilosophers]);
       printf("Philosopher %d put down utensil %d.\n", id, first % numPhilosophers);
       
       pthread_mutex_unlock(&utensils[sec % numPhilosophers]);
       printf("Philosopher %d put down utensil %d.\n", id, sec % numPhilosophers);
       
    }
    return NULL;
}

int main() {
  /*
   * Initialize random number generator
   */
  srand(time(NULL));
  
  /*
   * Thread handles for each philosopher
   */
  pthread_t threads[numPhilosophers];
  
  /*
   * Array to store philosopher IDs to pass to threads
   */
  int ids[numPhilosophers];
  
  /*
   * Initialize mutexes for all utensils
   */
  for (int i = 0; i < numPhilosophers; i++)
    pthread_mutex_init(&utensils[i], NULL);
  
  /*
   * Create a thread for each philosopher
   */
  for (int i = 0; i < numPhilosophers; i++) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, philosopher, &ids[i]);
  }
  
  /*
   * Wait for threads to complete (never happens in this example,
   * though the program shouldn't deadlock)
   * 
   * TEACHING POINT: Unlike philo1.c and philo2.c, this implementation
   * has guaranteed deadlock prevention through resource hierarchy.
   * By ensuring at least one philosopher follows a different acquisition
   * order, circular wait is impossible.
   */
  for (int i = 0; i < numPhilosophers; i++)
    pthread_join(threads[i], NULL);

  /*
   * Clean up mutexes (never reached in this example since the program runs forever)
   */
  for (int i = 0; i < numPhilosophers; i++)
    pthread_mutex_destroy(&utensils[i]);
  
  return 0;
}