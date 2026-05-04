#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// dining philosophers, naive version
// 5 philosophers sit at a round table, 5 forks between them
// each one needs the fork on their LEFT and the fork on their RIGHT to eat
// they all reach for the left fork first, then the right fork
// then eat, then put the forks down

// what goes wrong:
// every philosopher picks up their left fork at roughly the same time
// now every fork is held by somebody waiting for their right neighbor's fork
// nobody can ever get a second fork
// classic deadlock -- run this and watch the program freeze after the first round

#define numPhilosophers 5

pthread_mutex_t forks[numPhilosophers];

void* philosopher(void* num){
  int id = *(int*)num;

  while(1){
    printf("Philosopher %d is thinking.\n", id);
    sleep(1);

    // grab left fork
    pthread_mutex_lock(&forks[id]);
    printf("Philosopher %d picked up left fork %d.\n", id, id);

    // pause here so we're sure the deadlock has time to set up
    sleep(1);

    // grab right fork
    pthread_mutex_lock(&forks[(id+1) % numPhilosophers]);
    printf("Philosopher %d picked up right fork %d.\n", id, (id+1) % numPhilosophers);

    printf("Philosopher %d is eating.\n", id);
    sleep(1);

    pthread_mutex_unlock(&forks[(id+1) % numPhilosophers]);
    pthread_mutex_unlock(&forks[id]);
  }
  return NULL;
}

int main(){
  pthread_t threads[numPhilosophers];
  int ids[numPhilosophers];

  printf("My name is %d\n",getpid());
  
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
