#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

long counter = 0;

pthread_mutex_t counterMutex;

void* inc(void* arg){
  for(int i = 0; i < 100000; i++){
    pthread_mutex_lock(&counterMutex);
    counter++;
    pthread_mutex_unlock(&counterMutex);
  }
  return NULL;
}

int main() {
    pthread_t threads[1000];

    pthread_mutex_init(&counterMutex, NULL);
    for (long t = 0; t < 1000; t++) {
        pthread_create(&threads[t], NULL, inc, NULL);
    }
    for (int t = 0; t < 1000; t++) {
        pthread_join(threads[t], NULL);
    }

    // Destroy the mutex
    pthread_mutex_destroy(&counterMutex);

    // Print the final counter value
    printf("Final counter value: %ld (Expected: %d)\n", counter, 1000 * 100000);
    pthread_exit(NULL);
}
