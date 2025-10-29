#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int counter = 0;

void* incer(void* arg){

  int temp = counter;
  sleep(rand()%3+1); // simulated work
  counter = temp+1;
  
  return NULL;
}

int main(){
  srand(time(NULL));
  pthread_t threads[100];
  for(int i=0; i<100;i++){
    pthread_create(threads+i,NULL,incer,NULL);
  }

  for(int i=0; i<100;i++){
    pthread_join(threads[i],NULL);
  }

  printf("The counter is: %d\n",counter);


  return 0;
}

  
