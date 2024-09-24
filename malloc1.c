#include <stdlib.h>
#include <stdio.h>

int main(){

  int* arr;

  int size;

  printf("Enter the size of the array to allocate: ");
  scanf("%d",&size);
  arr = (int*) malloc(size * sizeof(int));

  for(int i=0; i < size; i++){
    arr[i] = i*i;
  }

  for(int i=0; i < size; i++){
    printf("arr[%d] = %d\n",i,arr[i]);
  }

  free(arr);
  arr = NULL;
  
  return 0;
}
