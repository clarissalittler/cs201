#include "stdio.h"

int main(){

  int arr[5];

  void* weirdo = arr;

  printf("weirdo: %p\n",weirdo);
  printf("weirdo+1: %p\n",weirdo+1);
  // void star pointer arithmetic behaves
  // like a char
  
  return 0;
}
