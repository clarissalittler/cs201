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
 * TEACHING POINT: This implementation attempts to solve the deadlock problem
 * from philo1.c by introducing randomness in the order philosophers pick up
 * utensils. This approach is called "breaking symmetry" - preventing all
 * philosophers from following exactly the same deterministic pattern.
 */
void* philosopher(void* num) {
    /*
     * Extract the philosopher's ID from the argument
     */
    int id = *(int*)num;

    /*
     * Randomly determine whether to pick up utensils in order (id, id+1)
     * or in reverse order (id+1, id)
     * 
     * TEACHING POINT: This randomization breaks the symmetry that causes
     * deadlock. By having some philosophers pick up utensils in a different
     * order, it prevents the circular wait condition necessary for deadlock.
     */
    int chirality = rand() % 2;

    /*
     * Variables to hold the utensil indices based on random choice
     */
    int first;
    int sec;
    
    /*
     * Decide the order to pick up utensils based on random chirality
     */
    if(chirality){
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
      sleep(1);
      printf("Philosopher %d is thinking.\n", id);

      /*
       * Pick up first utensil (which could be either left or right)
       * 
       * TEACHING POINT: Unlike philo1.c, each philosopher might pick up
       * either their left or right utensil first, based on the random
       * chirality value. This reduces the likelihood of deadlock.
       */
      pthread_mutex_lock(&utensils[first % numPhilosophers]);
      printf("Philosopher %d picked up utensil %d .\n", id, first % numPhilosophers);

      sleep(1);
      printf("Philosopher %d is thinking.\n", id);
      
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
       * Put down utensils in the reverse order they were picked up
       * 
       * TEACHING POINT: This example still follows the proper release pattern:
       * resources should generally be released in the reverse order they were
       * acquired to minimize the potential for deadlock.
       */
      pthread_mutex_unlock(&utensils[sec % numPhilosophers]);
      printf("Philosopher %d put down utensil %d.\n", id, sec % numPhilosophers);

      pthread_mutex_unlock(&utensils[first % numPhilosophers]);
      printf("Philosopher %d put down utensil %d.\n", id, first % numPhilosophers);
        
    }
    return NULL;
}

int main() {
  /*
   * Initialize random number generator for the chirality decisions
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
   * though the program is less likely to deadlock than philo1.c)
   * 
   * TEACHING POINT: Even with randomization, this solution doesn't
   * completely eliminate the possibility of deadlock - it just makes
   * it less likely. True deadlock prevention requires more structured
   * approaches like resource hierarchy or limiting concurrency.
   */
  for (int i = 0; i < numPhilosophers; i++)
    pthread_join(threads[i], NULL);

  /*
   * Clean up mutexes (never reached in this example)
   */
  for (int i = 0; i < numPhilosophers; i++)
    pthread_mutex_destroy(&utensils[i]);
  
  return 0;
}