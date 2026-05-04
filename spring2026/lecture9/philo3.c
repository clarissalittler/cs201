#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// dining philosophers, fix #2: bound the number who can sit down
// every philosopher reaches left-then-right (the naive order!)
// but before they're allowed to start grabbing forks at all,
// they have to pass through a semaphore initialized to N-1 = 4
// so at any moment only 4 of the 5 philosophers are competing for forks
// at least one of those 4 must be able to get both forks
// no deadlock possible

// this is the use of a counting semaphore as a "resource cap" --
// same shape as the bouncer example, applied to the dining table

#define numPhilosophers 5

pthread_mutex_t forks[numPhilosophers];
sem_t seats; // how many philosophers may attempt to eat at once

void* philosopher(void* num){
  int id = *(int*)num;

  while(1){
    printf("Philosopher %d is thinking.\n", id);
    sleep(1);

    // ask permission to come to the table
    sem_wait(&seats);

    pthread_mutex_lock(&forks[id]);
    printf("Philosopher %d picked up left fork %d.\n", id, id);
    sleep(1);

    pthread_mutex_lock(&forks[(id+1) % numPhilosophers]);
    printf("Philosopher %d picked up right fork %d.\n", id, (id+1) % numPhilosophers);

    printf("Philosopher %d is eating.\n", id);
    sleep(1);

    pthread_mutex_unlock(&forks[(id+1) % numPhilosophers]);
    pthread_mutex_unlock(&forks[id]);

    // leave the table -- somebody else can come sit down
    sem_post(&seats);
  }
  return NULL;
}

int main(){
  pthread_t threads[numPhilosophers];
  int ids[numPhilosophers];

  for(int i=0; i<numPhilosophers; i++)
    pthread_mutex_init(&forks[i], NULL);

  // N - 1 slots: one fewer philosopher than there are forks
  sem_init(&seats, 0, numPhilosophers - 1);

  for(int i=0; i<numPhilosophers; i++){
    ids[i] = i;
    pthread_create(&threads[i],NULL,philosopher,&ids[i]);
  }

  for(int i=0; i<numPhilosophers; i++)
    pthread_join(threads[i],NULL);

  for(int i=0; i<numPhilosophers; i++)
    pthread_mutex_destroy(&forks[i]);

  sem_destroy(&seats);
  return 0;
}
