#include "stdio.h"

// two big reasons for pointers:
// 1. to do dynamic memory (malloc / free)
// 2. because there are no pass-by-reference parameters in C!

void worldsWorstInc(int* p){
  *p = *p + 1;
}


int main(){

  // this is a variable, it lives *somewhere*
  int num = 0;
  // we can check where it lives!
  printf("%p\n",&num);
  int* np = &num;

  printf("The variable with address %p has a value of %d\n", np, *np);

  worldsWorstInc(np);
  worldsWorstInc(np);

  printf("The variable with address %p now has a value of %d\n", np, *np); 
  
  return 0;
}
