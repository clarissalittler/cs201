#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define numPhilosophers 5

pthread_mutex_t utensils[numPhilosophers];

void* philosopher(void* num) {
    int id = *(int*)num;

    while (1) {
        // Pick up left utensil
      sleep(1);
      printf("Philosopher %d is thinking.\n", id);
      pthread_mutex_lock(&utensils[id]);
      printf("Philosopher %d picked up utensil %d (left).\n", id, id);
      sleep(1);
      printf("Philosopher %d is thinking.\n", id);
      
      
      // Pick up right utensil
      pthread_mutex_lock(&utensils[(id + 1) % numPhilosophers]);
      printf("Philosopher %d picked up utensil %d (right).\n", id, (id + 1) % numPhilosophers);
      
      // Eating
      printf("Philosopher %d is eating.\n", id);
      sleep(1);
      
      // Put down right utensil
      pthread_mutex_unlock(&utensils[(id + 1) % numPhilosophers]);
      printf("Philosopher %d put down utensil %d (right).\n", id, (id + 1) % numPhilosophers);
      
        // Put down left utensil
      pthread_mutex_unlock(&utensils[id]);
      printf("Philosopher %d put down utensil %d (left).\n", id, id);
    }
    return NULL;
}

int main() {
    pthread_t threads[numPhilosophers];
    int ids[numPhilosophers];

    // Initialize mutexes
    for(int i = 0; i < numPhilosophers; i++)
        pthread_mutex_init(&utensils[i], NULL);

    // Create philosopher threads
    for(int i = 0; i < numPhilosophers; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    // Join threads (never reached in this example)
    for(int i = 0; i < numPhilosophers; i++)
        pthread_join(threads[i], NULL);

    // Destroy mutexes (never reached in this example)
    for(int i = 0; i < numPhilosophers; i++)
        pthread_mutex_destroy(&utensils[i]);

    return 0;
}
