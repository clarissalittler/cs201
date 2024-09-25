#include <stdio.h>
#include <stdlib.h>

int main(){

  // need malloc and sizeof
  int* arr = (int*) malloc(10 * sizeof(int));

  for(int i = 0; i < 10; i++){
    arr[i] = i*i; // *(arr + i) = i * i;
  }

  for(int i = 0; i < 10; i++){
    printf("arr[%d] = %d\n",i,arr[i]);
  }
  
  // Dynamically make an array of ten ints
  // use a for loop (for loops are just like c++)
  // to fill the array so that arr[i] = i*i;
  // then print it with printf element by element
  // in another loop

  free(arr);
  
  return 0;
}
