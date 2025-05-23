#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
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
 * Semaphore to limit the number of philosophers that can eat at once
 * 
 * TEACHING POINT: This demonstrates another solution to the dining philosophers
 * problem: limiting the number of philosophers who can try to acquire utensils
 * simultaneously. By allowing at most 4 philosophers (out of 5) to compete for
 * resources, deadlock is avoided.
 */
sem_t semmy;


/*
 * Philosopher thread function
 * 
 * TEACHING POINT: This implementation uses the original algorithm from philo1.c
 * (where each philosopher picks up left then right utensil), but adds a semaphore
 * to ensure that at most 4 philosophers can be in the "trying to eat" state at once.
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
      sleep(1);

      /*
       * Wait on the semaphore before trying to pick up any utensils
       * 
       * TEACHING POINT: This is the key difference from philo1.c. The semaphore
       * ensures that at most 4 philosophers can be in the critical "trying to eat"
       * section at the same time. This breaks the circular dependency that causes
       * deadlock, because at least one philosopher will be completely out of the
       * competition for resources.
       */
      sem_wait(&semmy);
      
      /*
       * Pick up left utensil
       */
      printf("Philosopher %d is thinking.\n", id);
      pthread_mutex_lock(&utensils[id]);
      printf("Philosopher %d picked up utensil %d (left).\n", id, id);
      sleep(1);
      printf("Philosopher %d is thinking.\n", id);
      
      
      /*
       * Pick up right utensil
       * 
       * TEACHING POINT: Even though all philosophers follow the same order
       * (left then right), deadlock is prevented because the semaphore ensures
       * that at most 4 philosophers are trying to acquire utensils at once.
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
      
      /*
       * Signal the semaphore to let another philosopher try to eat
       * 
       * TEACHING POINT: This increases the semaphore count, allowing another
       * waiting philosopher to enter the critical section. This must be done
       * after putting down both utensils, ensuring that resources are fully
       * released before allowing another philosopher to compete for them.
       */
      sem_post(&semmy);
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
     * Initialize the semaphore with value 4
     * 
     * TEACHING POINT: This allows at most 4 philosophers (out of 5) to
     * attempt to acquire utensils at the same time. This is sufficient to
     * prevent deadlock, as deadlock requires all 5 philosophers to be
     * involved in a circular wait.
     */
    sem_init(&semmy,0,4);

    
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
     * Wait for threads to complete (never happens in this example
     * because the philosophers loop forever)
     */
    for (int i = 0; i < numPhilosophers; i++)
        pthread_join(threads[i], NULL);

    /*
     * Clean up mutexes (never reached in this example)
     */
    for (int i = 0; i < numPhilosophers; i++)
        pthread_mutex_destroy(&utensils[i]);

    /*
     * Clean up the semaphore (never reached in this example)
     */
    sem_destroy(&semmy);
    
    return 0;
}