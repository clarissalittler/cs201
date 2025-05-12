#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#define numPhilosophers 5

pthread_mutex_t utensils[numPhilosophers];
sem_t semmy;


void* philosopher(void* num) {
    int id = *(int*)num;

    while (1) {
      sleep(1);

      sem_wait(&semmy);
        // Pick up left utensil
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
      sem_post(&semmy);
    }
    return NULL;
}

int main() {
    pthread_t threads[numPhilosophers];
    int ids[numPhilosophers];
    sem_init(&semmy,0,4);

    
    // Initialize mutexes
    for (int i = 0; i < numPhilosophers; i++)
        pthread_mutex_init(&utensils[i], NULL);

    // Create philosopher threads
    for (int i = 0; i < numPhilosophers; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    
    for (int i = 0; i < numPhilosophers; i++)
        pthread_join(threads[i], NULL);

    for (int i = 0; i < numPhilosophers; i++)
        pthread_mutex_destroy(&utensils[i]);

    sem_destroy(&semmy);
    
    return 0;
}
