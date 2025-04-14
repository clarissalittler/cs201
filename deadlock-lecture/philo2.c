#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define numPhilosophers 5

pthread_mutex_t utensils[numPhilosophers];

void* philosopher(void* num) {
    int id = *(int*)num;

    int chirality = rand() % 2;

    int first;
    int sec;
    
    if(chirality){
      first = id;
      sec = id+1;
    }
    else {
      first = id+1;
      sec = id;
    }
    
    while (1) {

      sleep(1);
      printf("Philosopher %d is thinking.\n", id);

      // Pick up left utensil
      pthread_mutex_lock(&utensils[first % numPhilosophers]);
      printf("Philosopher %d picked up utensil %d .\n", id, first % numPhilosophers);

      sleep(1);
      printf("Philosopher %d is thinking.\n", id);
      
      // Pick up right utensil
      pthread_mutex_lock(&utensils[sec % numPhilosophers]);
      printf("Philosopher %d picked up utensil %d .\n", id, sec % numPhilosophers);
      
        // Eating
      printf("Philosopher %d is eating.\n", id);
      sleep(1);
      
      // Put down right utensil
      pthread_mutex_unlock(&utensils[sec % numPhilosophers]);
      printf("Philosopher %d put down utensil %d.\n", id, sec % numPhilosophers);

      // Put down left utensil
      pthread_mutex_unlock(&utensils[first % numPhilosophers]);
      printf("Philosopher %d put down utensil %d.\n", id, first % numPhilosophers);
        
    }
    return NULL;
}

int main() {

  srand(time(NULL));
  pthread_t threads[numPhilosophers];
  int ids[numPhilosophers];
  
  // Initialize mutexes
  for (int i = 0; i < numPhilosophers; i++)
    pthread_mutex_init(&utensils[i], NULL);
  
  // Create philosopher threads
  for (int i = 0; i < numPhilosophers; i++) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, philosopher, &ids[i]);
  }
  
  // Join threads (never reached in this example)
  for (int i = 0; i < numPhilosophers; i++)
    pthread_join(threads[i], NULL);

  // Destroy mutexes (never reached in this example)
  for (int i = 0; i < numPhilosophers; i++)
    pthread_mutex_destroy(&utensils[i]);
  
  return 0;
}
