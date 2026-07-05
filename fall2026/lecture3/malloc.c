#include <stdio.h>
#include <stdlib.h>

int main(){

  //malloc always returns a pointer
  //malloc needs explicit sizes
  int* n = malloc(sizeof(int));

  *n = 10;

  printf("%d\n",*n);

  free(n);
  //n = NULL;
  n = 0;
  if(0 == NULL){
    printf("Yup\n");
  }
  
  
  return 0;
}
