#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int ourCounter = 0;

pthread_mutex_t mutex;


void* threadCounter(void* arg){
  pthread_mutex_lock(&mutex);
  int temp = ourCounter;
  sleep(rand()%3);
  ourCounter = temp+1;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(){
  srand(time(0));
  pthread_t threads[10];

  // the second argument is for configuring it
  pthread_mutex_init(&mutex,NULL);

  
  for(int i=0;i<10;i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  for(int i=0; i< 10; i++){
    pthread_join(threads[i],NULL);
  }

  pthread_mutex_destroy(&mutex);

  printf("What's the value of this counter? %d\n",ourCounter);
  return 0;
}
