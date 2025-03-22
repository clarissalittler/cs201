#include <stdio.h>
#include <stdlib.h>

int main(){

  int* bigArray = (int*) malloc(100000 * sizeof(int));

  for(int i = 0; i < 100000; i++){
    bigArray[i] = i*i;
  }

  for(int i = 0; i < 100000; i++){
    printf("bigArray[%d] = %d\n",i,bigArray[i]);
  }

  printf("The address of bigArray is: %p\n",bigArray);
  printf("Oh and btw sizeof(int) is: %ld\n",sizeof(int));

  free(bigArray);
  bigArray = NULL;
  printf("Now the address of bigArray is: %p\n",bigArray);
  
  return 0;
}
