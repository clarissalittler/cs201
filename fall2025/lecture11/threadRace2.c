#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


int counter = 0;

pthread_mutex_t mutex;


void* incer(void* arg){
  pthread_mutex_lock(&mutex); // only one thread will win, everyone else waits until the mutex is *un* locked
  
  int temp = counter;
  sleep(rand()%3+1); // simulated work
  counter = temp+1;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(){
  srand(time(NULL));
  pthread_t threads[5];
  pthread_mutex_init(&mutex,NULL); // pass the pointer to the mutex
  
  for(int i=0; i<5;i++){
    pthread_create(threads+i,NULL,incer,NULL);
  }

  for(int i=0; i<5;i++){
    pthread_join(threads[i],NULL);
  }

  printf("The counter is: %d\n",counter);
  pthread_mutex_destroy(&mutex);

  return 0;
}

  
