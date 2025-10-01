#include "stdio.h"

int main(){

  int arr[10];
  for(int i = 0; i < 10; i++){
    arr[i] = i*i;
  }

  printf("If we don't cast at all arr+1 is: %p\n", arr+1);

  printf("If we cast to long, arr+1 is: %p\n", ((long*)arr) + 1);

  printf("If we cast to char, arr+1 is: %p\n", ((char*)arr) + 1);
  
  return 0;
}
