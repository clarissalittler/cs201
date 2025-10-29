#include "stdlib.h"
#include "stdio.h"

int addWrongly(int a, int b){
  return (2*a + b);
}

int incWrongly(int a){
  return (a + 2);
}

// let's write a function that will take
// an array of integers and a function that takes a single integer and returns an integer, and applies this function on every element of the array
// map([1,2,3,4],4,incWrongly) => turn our array into [3,4,5,6]

void map(int arr[], int sz,int (*func)(int)){
  for(int i=0;i<sz;i++){
    arr[i] = func(arr[i]);
  }  
}

void printArray(int arr[], int sz){
  printf("[");
  for(int i=0;i<sz;i++){
    printf("%d,",arr[i]);
  }
  printf("]\n");
}

int main(){

  int dummy[5] = {0,1,2,3,4};
  printArray(dummy,5);
  map(dummy,5,incWrongly);
  printArray(dummy,5);
  
  return 0;
}
