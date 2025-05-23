#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <semaphore.h>

/*
 * Number of philosophers (and utensils) in the simulation
 */
#define numPhilosophers 5

/*
 * Array of mutexes, one for each utensil
 */
pthread_mutex_t utensils[numPhilosophers];

/*
 * Semaphore to control access to the dining table
 * 
 * TEACHING POINT: This is declared but never initialized or used in this example.
 * It appears to be a leftover from an implementation that would have used a semaphore
 * to limit the number of philosophers who could try to eat simultaneously (another
 * solution to the dining philosophers problem).
 * 
 * This illustrates a common issue in software development - code remnants that
 * don't actually participate in the program's functionality.
 */
sem_t monitor;


/*
 * Philosopher thread function
 * 
 * TEACHING POINT: This is identical to philo3.c, using the resource hierarchy
 * solution. The last philosopher picks up utensils in reverse order to break
 * the circular wait condition that would cause deadlock.
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
        * Put down utensils in the same order they were acquired
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
   * Wait for threads to complete (never happens in this example)
   */
  for (int i = 0; i < numPhilosophers; i++)
    pthread_join(threads[i], NULL);

  /*
   * Clean up mutexes (never reached in this example)
   */
  for (int i = 0; i < numPhilosophers; i++)
    pthread_mutex_destroy(&utensils[i]);
  
  /*
   * TEACHING POINT: This code is missing cleanup for the semaphore
   * (sem_destroy(&monitor)) because the semaphore was never initialized.
   * This would be a resource leak if the semaphore had been initialized.
   */
  
  return 0;
}