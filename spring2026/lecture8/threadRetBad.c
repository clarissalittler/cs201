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
  int p; // this creates a variable *on the stack frame* of the function (something like -4(%rbp))
  

  p = *((int*)i)+1;
  
  return &p; // returns the value at -4(%rbp)
  // after this point, we've wiped out the stack frame
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
  // at this point retVal has the value of -4(oldrbp)

  // inside printf we dereference that
  // which ends up pointing at junk values
  // or even something that the program is not allowed to
  // dereference, like a place inside the text segment
  printf("The returned value was: %d\n",*((int*)retVal));
  
  return 0;
}
