#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// in this program we're going to demonstrate how to
// return values *out* of threaded code

// take in the passed in value and return that +1
void* worstIncrement(void* i){
  // we take in the argument
  // and we need to
  // 1. make space for the return value (malloc)
  // 2. put the value to be returned in it
  // 3. return the pointer
  int* p = malloc(sizeof(int));

  *p = *((int*)i)+1;
  
  return p;
}

int main(){

  pthread_t thread;
  int argVal = 10;
  // it's easier to just make your return value a
  // void*
  void* retVal;
  // reminder: pthread_create : thread data structure,
  //                            configuration,
  //                            the function to run
  //                            pointer to argument
  pthread_create(&thread,NULL,worstIncrement,&argVal);
  pthread_join(thread,&retVal);

  printf("The returned value was: %d\n",*((int*)retVal));
  
  return 0;
}
