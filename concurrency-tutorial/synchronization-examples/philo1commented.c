#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/*
 * Number of philosophers (and utensils) in the simulation
 */
#define numPhilosophers 5

/*
 * Array of mutexes, one for each utensil
 * 
 * TEACHING POINT: This demonstrates using mutexes to represent physical resources
 * (utensils in this case). A philosopher must acquire the mutex to "pick up" the
 * corresponding utensil, and release it to "put down" the utensil.
 */
pthread_mutex_t utensils[numPhilosophers];

/*
 * Philosopher thread function
 * 
 * TEACHING POINT: This implements the classic Dining Philosophers problem,
 * a famous synchronization scenario used to illustrate concurrent programming
 * challenges. Each philosopher needs two utensils to eat, creating potential
 * for deadlock if everyone picks up their left utensil and waits for the right one.
 */
void* philosopher(void* num) {
    /*
     * Extract the philosopher's ID from the argument
     */
    int id = *(int*)num;

    /*
     * Philosophers loop forever between thinking and eating
     */
    while (1) {
        // Pick up left utensil
      sleep(1);
      printf("Philosopher %d is thinking.\n", id);
      
      /*
       * Attempt to pick up left utensil (the one with the same number as the philosopher)
       * 
       * TEACHING POINT: This is where deadlock potential begins - if all philosophers
       * pick up their left utensil simultaneously, they'll all be waiting for their right
       * utensil which someone else already holds.
       */
      pthread_mutex_lock(&utensils[id]);
      printf("Philosopher %d picked up utensil %d (left).\n", id, id);
      sleep(1);
      printf("Philosopher %d is thinking.\n", id);
      
      
      /*
       * Attempt to pick up right utensil (the one with the number (id+1) % numPhilosophers)
       * 
       * TEACHING POINT: This implementation will eventually deadlock because:
       * 1. All philosophers try to pick up their left utensil first
       * 2. Then they all try to pick up their right utensil
       * 3. Since each utensil is shared between two philosophers, this creates a circular
       *    waiting condition - the definition of deadlock
       */
      pthread_mutex_lock(&utensils[(id + 1) % numPhilosophers]);
      printf("Philosopher %d picked up utensil %d (right).\n", id, (id + 1) % numPhilosophers);
      
      /*
       * Eat for a moment now that both utensils are acquired
       */
      printf("Philosopher %d is eating.\n", id);
      sleep(1);
      
      /*
       * Put down right utensil
       */
      pthread_mutex_unlock(&utensils[(id + 1) % numPhilosophers]);
      printf("Philosopher %d put down utensil %d (right).\n", id, (id + 1) % numPhilosophers);
      
      /*
       * Put down left utensil
       */
      pthread_mutex_unlock(&utensils[id]);
      printf("Philosopher %d put down utensil %d (left).\n", id, id);
    }
    return NULL;
}

int main() {
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
    for(int i = 0; i < numPhilosophers; i++)
        pthread_mutex_init(&utensils[i], NULL);

    /*
     * Create a thread for each philosopher
     * 
     * TEACHING POINT: This launches all philosopher threads simultaneously,
     * allowing them to compete for resources in ways that can lead to deadlock.
     */
    for(int i = 0; i < numPhilosophers; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    /*
     * Wait for threads to complete (never happens in this example)
     * 
     * TEACHING POINT: This program will deadlock before reaching this point,
     * which illustrates why the comments say this code is "never reached."
     * The deadlock occurs when each philosopher has acquired their left utensil
     * and is waiting for their right utensil.
     */
    for(int i = 0; i < numPhilosophers; i++)
        pthread_join(threads[i], NULL);

    /*
     * Clean up mutexes (never happens in this example)
     */
    for(int i = 0; i < numPhilosophers; i++)
        pthread_mutex_destroy(&utensils[i]);

    return 0;
}