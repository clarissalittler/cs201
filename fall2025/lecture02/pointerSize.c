#include "stdio.h"
#include "stdlib.h"

int main(){
  int num1;
  float num2;
  long num3;
  double num4;
  printf("The size of an int* is: %ld\n",sizeof(&num1));
  printf("The size of a long* is: %ld\n",sizeof(&num3));
  printf("The size of a float* is: %ld\n",sizeof(&num2));
  printf("The size of a double* is: %ld\n",sizeof(&num4));
  
  
  return 0;
}
