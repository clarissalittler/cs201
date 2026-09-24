#include <stdio.h>
#include <stdlib.h>

int main(){

  //malloc always returns a pointer
  //malloc needs explicit sizes
  int* n = malloc(sizeof(int));
  int* arr = malloc(10*sizeof(int));
  
  *n = 10;

  printf("%d\n",*n);

  free(n);
  free(arr);
  arr = 0;
  //n = NULL;
  n = 0;
  if(0 == NULL){
    printf("Yup\n");
  }
  
  
  return 0;
}
