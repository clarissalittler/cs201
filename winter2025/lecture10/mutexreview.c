#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct protectedCounter{
  int counter;
  pthread_mutex_t mutex;
};

void* threadCounter(void* arg){
  struct protectedCounter* c = (struct protectedCounter*) arg;
  pthread_mutex_lock(&c->mutex);
  sleep(rand()%3+1);
  c->counter = c->counter+1;
  pthread_mutex_unlock(&c->mutex);  
  return NULL;
}

int main(){
  srand(time(NULL));
  pthread_t threads[10];
  struct protectedCounter c1;
  c1.counter = 0;
  pthread_mutex_init(&c1.mutex,NULL);
  struct protectedCounter c2;
  c2.counter = 0;
  pthread_mutex_init(&c2.mutex,NULL);
  for(int i =0; i < 10; i++){
    if(i % 2 == 1){
      pthread_create(&threads[i],NULL,threadCounter,&c1);
    }
    else{
      pthread_create(&threads[i],NULL,threadCounter,&c2);
    }
  }

  for(int i=0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  printf("The value of our first counter is: %d\n",c1.counter);
  printf("The value of our second counter is: %d\n",c2.counter);

  pthread_mutex_destroy(&c1.mutex);
  pthread_mutex_destroy(&c2.mutex);
  return 0;
}




