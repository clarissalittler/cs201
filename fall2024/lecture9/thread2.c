#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

int ourCounter = 0;

sem_t ourSem;

void* threadCounter(void* arg){
  sem_wait(&ourSem);
  int temp = ourCounter;
  sleep(rand()%3);
  ourCounter = temp+1;
  sem_post(&ourSem);
  return NULL;
}

int main(){
  srand(time(0));
  pthread_t threads[10];

  sem_init(&ourSem,0,1);

  for(int i = 0;i < 10; i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  for(int i =0; i < 10;i++){
    pthread_join(threads[i],NULL);
  }

  sem_destroy(&ourSem);
  
  printf("What's the value of this counter?? %d\n",ourCounter);
  return 0;
}
