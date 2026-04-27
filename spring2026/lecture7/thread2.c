#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* threadFun(void* p){
  int* arg = (int*)p;
  (*arg)++;  
  return NULL;
}

// pattern you might not have seen before:
// in many programming languages there's a Maybe
// or an Option
// type that represent you either /have/ a thing or you /don't/
// in (old-hat) C, you have to represent this with pointer shenanigans
// since a pointer can be NULL

int main(){

  int counter = 0;
  
  // first we make the structure to hold
  // the thread's information
  pthread_t thread;
  // next, we need to launch the thread by passing this structure (by reference) to pthread_create
  pthread_create(&thread,NULL,threadFun,&counter);

  pthread_join(thread,NULL);
  printf("Counter is: %d\n",counter);  
  
  return 0;
}
