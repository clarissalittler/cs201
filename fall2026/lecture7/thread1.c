#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// why all the void*s?
// Well this is how we get polymorphism in C
// any kind of pointer can get cast to a void*
// and if you *know* what kind of data it's supposed to be, you can safely cast it back
// the point here is that the threading functions that launch and manage the threads
// need to be able to handle functions as arguments that can take anything as an argument
// and potentially return anything

void* threadFun(void* p){
  printf("Hi from a thread!\n");
  
  return NULL;
}

// pattern you might not have seen before:
// in many programming languages there's a Maybe
// or an Option
// type that represent you either /have/ a thing or you /don't/
// in (old-hat) C, you have to represent this with pointer shenanigans
// since a pointer can be NULL

int main(){

  // first we make the structure to hold
  // the thread's information
  pthread_t thread;
  // next, we need to launch the thread by passing this structure (by reference) to pthread_create
  pthread_create(&thread,NULL,threadFun,NULL);

  pthread_join(thread,NULL);
  
  
  return 0;
}
