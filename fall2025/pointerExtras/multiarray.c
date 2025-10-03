#include "stdio.h"
#include "stdlib.h"

// here's the pointers->pointers way
// of doing dynamically allocated
// multi-dimensional arrays

int main(){

  // 10x10
  int** arr = malloc(10*sizeof(int*));
  for(int i = 0; i<10; i++){
    // arr[i] IS REALLY *(arr + i)
    arr[i] = malloc(10*sizeof(int)); 
  }

  for(int i=0; i < 10; i++){
    for(int j=0; j < 10; j++){
      arr[i][j] = 100*(i+1) + j; 
    }
  }

  for(int i=0; i < 10; i++){
    for(int j=0; j < 10; j++){
      printf("%d ", arr[i][j]);
    }
    printf("\n");
  }
  
  return 0;
}
