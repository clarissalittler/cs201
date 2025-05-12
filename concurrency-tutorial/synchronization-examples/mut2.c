#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct threadData {
  long resource;
  pthread_mutex_t mut;
};

void* inc(void* arg){
  struct threadData* d = (struct threadData*) arg;
  for(int i = 0; i < 100000; i++){
    pthread_mutex_lock(&d->mut);
    d->resource++;
    pthread_mutex_unlock(&d->mut);
  }
  return NULL;
}

int main(){

  pthread_t threads[1000];

  struct threadData ev;
  struct threadData odd;
  ev.resource = 0;
  odd.resource = 0;
  pthread_mutex_init(&ev.mut, NULL);
  pthread_mutex_init(&odd.mut, NULL);
  
  for (int i = 0; i < 1000; i++) {
    if(i % 2 == 0){
      pthread_create(&threads[i], NULL, inc, &ev);
    }
    else{
      pthread_create(&threads[i], NULL, inc, &odd);
    }
  }
  for (int i = 0; i < 1000; i++) {
    pthread_join(threads[i], NULL);
  }
  
  // Destroy the mutex
  pthread_mutex_destroy(&ev.mut);
  pthread_mutex_destroy(&odd.mut);

  // Print the final counter value
  printf("Final even counter value: %ld (Expected: %d)\n", ev.resource, 500 * 100000);
  printf("Final odd counter value: %ld (Expected: %d)\n", odd.resource, 500 * 100000);
  pthread_exit(NULL);  
}
