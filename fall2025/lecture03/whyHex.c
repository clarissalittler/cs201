#include "stdio.h"

int main(){

  int num1;

  // *(pointer + i) = pointer[i]
  
  printf("The address of num1 is: %p\n",&num1);
  
  printf("The address of &num1 + 1 is: %p \n",&num1 + 1);
  
  return 0;
}
