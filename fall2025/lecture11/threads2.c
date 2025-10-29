#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"

/*
  void and void* mean extremely different things

  void is the type of nothing

  void* is the universal donor of pointer types
  anything can be cast to void*
  you can, at your own risk, cast void* to anything
  (casting with pointers is *weird*)
  
 */

// void* is the only way to get polymorphism out of C
// the pthread_create function
// has to take the most generic possible
// function type as an argument
// and that's a thing that takes a void*
// and returns a void*
void* func(void* arg){

  // now arg is SECRETLY a int*
  printf("Hi I'm a thread and I'm holding: %d\n",
	 *((int*)arg));
  
  return NULL;
}

int main(){
  
  pthread_t thread; // the data structure that holds all the information about a thread

  int arg = 10;
  
  pthread_create(&thread,NULL,func,&arg);
  // the third argument of pthread_create
  // is a function!
  // hence why I was talking about function pointers
  // it's not (entirely) because I'm whacked out with poo-brain
  pthread_join(thread,NULL);
  

  return 0;
}
