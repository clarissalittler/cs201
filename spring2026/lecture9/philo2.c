#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// dining philosophers, fix #1: break the cycle
// 4 of the philosophers reach left-then-right, just like before
// but the LAST one reaches right-then-left
// because everyone else's "left" is somebody else's "right",
// this means there is no longer a circular wait:
// the cycle in the resource graph is broken at exactly one spot
// (this is one of the 4 deadlock conditions getting taken away)

#define numPhilosophers 5

pthread_mutex_t forks[numPhilosophers];

void* philosopher(void* num){
  int id = *(int*)num;
  int first, second;

  // everyone but the last grabs left then right
  // the last one grabs right then left
  if(id != numPhilosophers - 1){
    first  = id;
    second = (id+1) % numPhilosophers;
  } else {
    first  = (id+1) % numPhilosophers;
    second = id;
  }

  while(1){
    printf("Philosopher %d is thinking.\n", id);
    sleep(1);

    pthread_mutex_lock(&forks[first]);
    printf("Philosopher %d picked up fork %d.\n", id, first);
    sleep(1);

    pthread_mutex_lock(&forks[second]);
    printf("Philosopher %d picked up fork %d.\n", id, second);

    printf("Philosopher %d is eating.\n", id);
    sleep(1);

    pthread_mutex_unlock(&forks[second]);
    pthread_mutex_unlock(&forks[first]);
  }
  return NULL;
}

int main(){
  pthread_t threads[numPhilosophers];
  int ids[numPhilosophers];

  for(int i=0; i<numPhilosophers; i++)
    pthread_mutex_init(&forks[i], NULL);

  for(int i=0; i<numPhilosophers; i++){
    ids[i] = i;
    pthread_create(&threads[i],NULL,philosopher,&ids[i]);
  }

  for(int i=0; i<numPhilosophers; i++)
    pthread_join(threads[i],NULL);

  for(int i=0; i<numPhilosophers; i++)
    pthread_mutex_destroy(&forks[i]);

  return 0;
}
