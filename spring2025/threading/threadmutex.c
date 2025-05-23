#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

int ourCounter = 0;

pthread_mutex_t mutex;

void* threadCounter(void* arg){
  // we put the CRITICAL SECTION within the mutex lock/unlock
  pthread_mutex_lock(&mutex);
  int temp = ourCounter;
  sleep(rand()%3+1);
  ourCounter = temp+1;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(){
  srand(time(NULL));
  pthread_t threads[10];
  pthread_mutex_init(&mutex,NULL);
  
  for(int i=0; i < 10; i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  for(int i=0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }
  pthread_mutex_destroy(&mutex);
  
  printf("The value of ourCounter is: %d\n",ourCounter);
  
  return 0;   
}
        
