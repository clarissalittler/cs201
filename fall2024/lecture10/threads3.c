#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int ourCounter = 0;

pthread_mutex_t mutex;

void* threadCounter(void* arg){
  pthread_mutex_lock(&mutex);
  int temp = ourCounter;

  sleep(rand()%3);
  ourCounter = temp +1;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

void* wildCounter(void* arg){
  printf("Enter a number, right quick: ");
  int inc;
  scanf("%d",&inc);
  pthread_mutex_lock(&mutex);
  int temp = ourCounter;

  sleep(rand()%3);
  ourCounter = temp + inc;
  pthread_mutex_unlock(&mutex);
  return NULL;
}


int main(){
  srand(time(0));

  pthread_t threads[10];

  pthread_mutex_init(&mutex,NULL);
  
  for(int i = 0; i < 10; i++){
    if(i != 5){
      pthread_create(&threads[i],NULL,threadCounter,NULL);
    }
    else{
      pthread_create(&threads[i],NULL,wildCounter,NULL);
    }
  }

  for(int i = 0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  pthread_mutex_destroy(&mutex);
  
  printf("What's the value of the counter? %d\n",ourCounter);
  
  return 0;
}
