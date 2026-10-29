#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// this is the dining philosophers for 5 people
// the idea is that you have N people sitting around
// a circular table
// between each person is *one* utensil
// each philosopher needs *two* utensils in order
// to each
// the deadlock in the naive version of the problem
// happens when each philosopher reaches to their left
// to grab utensil and then reaches to their right
// to grab the second
// i.e. grabs the "left" mutex and then the "right" mutex

// solution 1, which is have even philosophers grab left then right, odd philosophers grab right then left

pthread_mutex_t forks[5];

void* dine(void* p){
  int id = *((int*)p);
  while(1){
  /*
    a philosopher is going to
    1. wait
    2. grab a left fork (id-1)
    3. wait
    4. grab a right fork (id)
    5. wait
    6. eat
    7. put down forks
   */
    printf("Philosopher %d is thinking...\n",id);
    sleep(rand()%3);
    int left; 
    if(id - 1 <0){
      left = 4;
    }
    else{
      left = id-1;
    }

    int right = id; 

    if(id % 2 == 0){
      pthread_mutex_lock(&forks[left]);
      printf("Philosopher %d has grabbed their left fork\n",id);
      sleep(rand()%2);
      pthread_mutex_lock(&forks[right]);
      printf("Philosopher %d has grabbed their right fork\n",id);
      sleep(rand()%2);
      printf("Philsopher %d is eating...\n",id);
      sleep(rand()%2);
      pthread_mutex_unlock(&forks[right]);
      pthread_mutex_unlock(&forks[left]);
    }
    else {
      sleep(rand()%2);
      pthread_mutex_lock(&forks[right]);
      printf("Philosopher %d has grabbed their right fork\n",id);
      pthread_mutex_lock(&forks[left]);
      printf("Philosopher %d has grabbed their left fork\n",id);
      sleep(rand()%2);
      printf("Philsopher %d is eating...\n",id);
      sleep(rand()%2);
      pthread_mutex_unlock(&forks[left]);
      pthread_mutex_unlock(&forks[right]);
    }
  }
    
  return NULL;
}

int main(){
  srand(time(NULL));
  // init our mutexes
  for(int i=0; i<5; i++){
    pthread_mutex_init(&forks[i],NULL);
  }

  pthread_t philos[5];
  int ids[5];
  // create the dining philosophers
  for(int i=0; i<5; i++){
    ids[i] = i;
    pthread_create(&philos[i],NULL,dine,&ids[i]);
    // BAD pthread_create(&philos[i],NULL,dine,&i)
  }

  // wait until they're all done
  for(int i=0; i<5; i++){
    pthread_join(philos[i],NULL);
  }
  
  return 0;
}
