#include <stdio.h>
#include <stdlib.h>

int main(){

  int* n = malloc(sizeof(int));

  *n = 10;

  printf("n contains %d\n",*n);
  
  free(n);
  n=0;
  
  printf("n contains %d\n",*n);
  
  return 0;
}
