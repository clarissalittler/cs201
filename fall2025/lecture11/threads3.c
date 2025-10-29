#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

// how do I get results *out* of a thread?
void* func(void* arg){

  int convArg = *((int*)arg);
  
  // now arg is SECRETLY a int*
  printf("Hi I'm a thread and I'm holding: %d\n",
	 *((int*)arg));

  int* ret = malloc(sizeof(int));

  *ret = 2*convArg;
  
  return ret;
}

int main(){
  
  pthread_t thread; // the data structure that holds all the information about a thread

  int arg = 10;
  int* res;
  
  pthread_create(&thread,NULL,func,&arg);
  // the third argument of pthread_create
  // is a function!
  // hence why I was talking about function pointers
  // it's not (entirely) because I'm whacked out with poo-brain
  // join takes the thread and a void**
  // why **?
  pthread_join(thread,(void**)&res);

  printf("The return value from the thread is: %d\n",*res);
  free(res);
  
  return 0;
}
