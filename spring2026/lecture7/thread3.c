#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* threadFun(void* p){
  int* arg = (int*)p;
  int temp = *arg;
  // the pattern of a lot of threading work
  // is that you take some data, then you operate on it
  // for awhile
  // then you put it back
  // the problem is that someone else may have put back
  // their work while you were doing yours
  for(int i=0; i<10000; i++){
    temp++;
  }
  // now we've done our work and we need to store
  // it back into memory
  *arg = temp;
  return NULL;
}

int main(){

  int counter = 0;
  
  // first we make the structure to hold
  // the thread's information
  pthread_t thread[100];
  // next, we need to launch the thread by passing this structure (by reference) to pthread_create
  for(int i=0; i<100; i++){
    pthread_create(&thread[i],NULL,threadFun,&counter);
  }

  for(int i=0; i<100; i++){
    pthread_join(thread[i],NULL);
  }
  
  printf("Counter is: %d\n",counter);
  
  return 0;
}
