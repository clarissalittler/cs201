#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int ourCounter = 0;

void* threadCounter(void* arg){
  int temp = ourCounter;
  sleep(rand()%3);
  ourCounter = temp+1;
  return NULL;
}

int main(){
  srand(time(0));
  pthread_t threads[10];

  for(int i = 0;i < 10; i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  for(int i =0; i < 10;i++){
    pthread_join(threads[i],NULL);
  }

  printf("What's the value of this counter?? %d\n",ourCounter);
  return 0;
}
