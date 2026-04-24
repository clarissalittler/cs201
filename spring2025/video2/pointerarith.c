#include <stdio.h>
#include <stdlib.h>

int main(){
  int arr1[10];
  double arr2[10];
  char arr3[10];

  printf("The addr of arr1 is %p and the addr of arr1+1 is %p\n",(void*)arr1,(void*)(arr1+1));
  printf("The addr of arr2 is %p and the addr of arr2+1 is %p\n",(void*)arr2,(void*)(arr2+1));
  printf("The addr of arr3 is %p and the addr of arr3+1 is %p\n",(void*)arr3,(void*)(arr3+1));

  
  return 0;
}
