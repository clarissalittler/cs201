#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/*
int counter = 0;
pthread_mutex_t mut;
*/

struct threadState {
  int counter;
  pthread_mutex_t mut;
};

void* threader(void* p){
  struct threadState* s = (struct threadState*)p;
  
  // extremely inefficient way to count to a million
  pthread_mutex_lock(&s->mut);
  int temp = s->counter;
  for(int i=0; i<10000; i++){
    temp++;
  }
  sleep(rand()%2);
  s->counter = temp;
  pthread_mutex_unlock(&s->mut);
  
  return NULL; // yes you could say return 0 but let's not rely on the punning of the many special 0s in C
}

int main(){

  srand(time(NULL));
  
  // pthread_mutex_init(&mut,NULL);
  
  struct threadState s;
  s.counter=0;
  pthread_mutex_init(&s.mut,NULL);
  
  pthread_t threads[100];

  for(int i=0; i<100; i++){
    pthread_create(&threads[i],NULL,threader,&s);
  }

  for(int i=0; i<100; i++){
    pthread_join(threads[i],NULL);
  }
  
  printf("Counter is: %d\n", s.counter);
  
  return 0;
}
