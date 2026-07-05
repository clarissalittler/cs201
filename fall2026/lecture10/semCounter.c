#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// the same counter-incrementer we wrote with a mutex last lecture
// only this time the protection is a binary semaphore
// a semaphore is like a mutex but with a counter:
// sem_wait decrements (and blocks if it would go negative)
// sem_post increments (and wakes a waiter, if there is one)
// initialized to 1, sem_wait/sem_post behave identically to lock/unlock
// (this is the "binary semaphore" usage)

long counter = 0;
sem_t s;

void* inc(void* arg){
  for(int i=0; i<100000; i++){
    sem_wait(&s); // "wait" waits until the internal counter is greater than 0
    counter++;
    sem_post(&s); // post adds 1 to the internal counter
  }
  return NULL;
}

int main(){
  pthread_t threads[1000];

  // 0 means "shared between threads of one process"
  // 1 is the starting value of the counter -- one thread allowed in at a time
  sem_init(&s,0,1);

  for(long t=0; t<1000; t++){
    pthread_create(&threads[t],NULL,inc,NULL);
  }
  for(int t=0; t<1000; t++){
    pthread_join(threads[t],NULL);
  }

  sem_destroy(&s);
  printf("Final counter value: %ld (Expected: %d)\n", counter, 1000*100000);
  return 0;
}
