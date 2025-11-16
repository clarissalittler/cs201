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

/*
 * ============================================================================
 * EXPECTED OUTPUT AND TESTING
 * ============================================================================
 *
 * TO COMPILE AND RUN:
 * gcc philo1.c -o philo1 -lpthread
 * ./philo1
 *
 * EXPECTED BEHAVIOR:
 * This program will DEADLOCK! It will not terminate normally.
 *
 * TYPICAL OUTPUT (before deadlock):
 * Philosopher 0 is thinking.
 * Philosopher 1 is thinking.
 * Philosopher 2 is thinking.
 * Philosopher 3 is thinking.
 * Philosopher 4 is thinking.
 * Philosopher 0 picked up utensil 0 (left).
 * Philosopher 1 picked up utensil 1 (left).
 * Philosopher 2 picked up utensil 2 (left).
 * Philosopher 3 picked up utensil 3 (left).
 * Philosopher 4 picked up utensil 4 (left).
 * [PROGRAM HANGS HERE - DEADLOCKED]
 *
 * WHAT HAPPENS:
 * 1. All 5 philosophers pick up their left utensil
 * 2. All try to pick up their right utensil
 * 3. But each right utensil is someone else's left utensil
 * 4. Everyone waits forever - DEADLOCK!
 *
 * VISUALIZATION OF DEADLOCK STATE:
 *      Utensil 0 - held by Philosopher 0, wanted by Philosopher 4
 *      Utensil 1 - held by Philosopher 1, wanted by Philosopher 0
 *      Utensil 2 - held by Philosopher 2, wanted by Philosopher 1
 *      Utensil 3 - held by Philosopher 3, wanted by Philosopher 2
 *      Utensil 4 - held by Philosopher 4, wanted by Philosopher 3
 *
 * CIRCULAR WAIT CONDITION:
 * P0 waits for P4 waits for P3 waits for P2 waits for P1 waits for P0
 * (This forms a cycle - the definition of deadlock!)
 *
 * TO EXIT THE DEADLOCKED PROGRAM:
 * Press Ctrl+C to send SIGINT and kill the process
 *
 * TESTING:
 * $ ./philo1
 * [Wait a few seconds... it will hang]
 * [Press Ctrl+C]
 * ^C
 * $
 *
 * LEARNING POINT:
 * This demonstrates why simply acquiring locks in the same order for all
 * threads is not sufficient if the order can create circular dependencies.
 *
 * SOLUTIONS:
 * See the other philosopher examples for different solutions:
 * - philo2.c: Randomized resource acquisition
 * - philo3.c: Resource hierarchy (break circular wait)
 * - philo4.c: Semaphore-based approach
 * - philo5.c: Resource manager (Dijkstra's solution)
 *
 * DEADLOCK CONDITIONS (all 4 must be present):
 * 1. Mutual exclusion - ✓ (mutexes can't be shared)
 * 2. Hold and wait - ✓ (holding left, waiting for right)
 * 3. No preemption - ✓ (can't forcibly take utensils)
 * 4. Circular wait - ✓ (P0→P1→P2→P3→P4→P0)
 *
 * Breaking ANY of these conditions prevents deadlock.
 */