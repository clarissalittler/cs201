#include <stdio.h>
#include <stdlib.h>

int main(){
  int* intPoint;
  intPoint = (int*) malloc(sizeof(int)); // void*

  *intPoint = 10;

  printf("the integer is: %d\n",*intPoint);

  free(intPoint);
  intPoint = NULL;

  printf("What is NULL: %p\n",NULL);
  
  return 0;
}
