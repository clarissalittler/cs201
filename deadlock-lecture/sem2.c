#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

const int maxConcurrent = 3;
const int totalThreads = 50;

sem_t sem;

void* handler(void* arg) {
    int id = *(int*)arg;
    sem_wait(&sem); // Acquire semaphore

    printf("Thread %d handling connection.\n", id);
    sleep(rand()%4 + 1); // Simulate work
    printf("Thread %d done.\n", id);

    sem_post(&sem); // Increments semaphore
    return NULL;
}

int main() {

  srand(time(0));
  pthread_t threads[totalThreads];
  int ids[totalThreads];

  sem_init(&sem, 0, maxConcurrent); 

  for (int i = 0; i < totalThreads; i++) {
    ids[i] = i + 1;
    pthread_create(&threads[i], NULL, handler, &ids[i]);
  }

  for (int i = 0; i < totalThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  sem_destroy(&sem); 
  return 0;
}
