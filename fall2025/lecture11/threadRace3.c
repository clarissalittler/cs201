#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


struct guardedCounter {
  int counter;
  pthread_mutex_t mutex;
};

void* incer(void* arg){
  struct guardedCounter* g = (struct guardedCounter*)arg;
  
  pthread_mutex_lock(&g->mutex); // only one thread will win, everyone else waits until the mutex is *un* locked
  
  int temp = g->counter;
  sleep(rand()%3+1); // simulated work
  g->counter = temp+1;
  pthread_mutex_unlock(&g->mutex);
  return NULL;
}

int main(){
  srand(time(NULL));
  pthread_t threads[5];
  struct guardedCounter c;
  c.counter = 0;
  
  pthread_mutex_init(&(c.mutex),NULL); // pass the pointer to the mutex
  
  for(int i=0; i<5;i++){
    pthread_create(threads+i,NULL,incer,&c);
  }

  for(int i=0; i<5;i++){
    pthread_join(threads[i],NULL);
  }

  printf("The counter is: %d\n",c.counter);
  pthread_mutex_destroy(&(c.mutex));

  return 0;
}

  
