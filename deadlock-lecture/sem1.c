#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

long counter = 0;

sem_t counterSem;

void* inc(void* arg){
  for(int i = 0; i < 100000; i++){
    sem_wait(&counterSem);
    counter++;
    sem_post(&counterSem);
  }
  return NULL;
}

int main() {
      pthread_t threads[1000];

      sem_init(&counterSem,0,1);
      for (long t = 0; t < 1000; t++) {
        pthread_create(&threads[t], NULL, inc, NULL);
      }
      for (int t = 0; t < 1000; t++) {
        pthread_join(threads[t], NULL);
      }

      // Destroy the mutex
      sem_destroy(&counterSem);

      // Print the final counter value
      printf("Final counter value: %ld (Expected: %d)\n", counter, 1000 * 100000);
      pthread_exit(NULL);
}
