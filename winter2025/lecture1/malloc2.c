#include <stdio.h>
#include <stdlib.h>

int main(){

  int* arr = malloc(10000000*sizeof(int));
  // creates an array that's 10000000 elements wide

  free(arr);
  
  return 0;
}
