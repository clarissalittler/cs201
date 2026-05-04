#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

// the *other* use of a semaphore: not as a mutex, but as a bouncer
// "at most N threads in this room at a time"
// here we have 50 threads simulating handling a connection,
// but the semaphore lets only 3 of them be "working" at once
// the other 47 sleep on sem_wait until somebody sem_posts

#define MAX_CONCURRENT 6
#define TOTAL_THREADS  50

sem_t s;

void* handler(void* arg){
  int id = *(int*)arg;
  sem_wait(&s); // grab a slot (blocks if all 3 are taken)

  printf("Thread %d handling connection.\n", id);
  sleep(rand()%4 + 1); // simulate work
  printf("Thread %d done.\n", id);

  sem_post(&s); // give the slot back
  return NULL;
}

int main(){
  srand(time(0));
  pthread_t threads[TOTAL_THREADS];
  int ids[TOTAL_THREADS];

  // start with MAX_CONCURRENT slots available
  sem_init(&s, 0, MAX_CONCURRENT);

  for(int i=0; i<TOTAL_THREADS; i++){
    ids[i] = i+1;
    pthread_create(&threads[i],NULL,handler,&ids[i]);
  }
  for(int i=0; i<TOTAL_THREADS; i++){
    pthread_join(threads[i],NULL);
  }

  sem_destroy(&s);
  return 0;
}
