#include <stdio.h>
#include <stdlib.h>

int main(){
  int arr1[10];
  double arr2[10];
  char arr3[10];

  printf("The addr of arr1 is %p and the addr of arr1+1 is %p\n",arr1,arr1+1);
  printf("The addr of arr2 is %p and the addr of arr2+1 is %p\n",arr2,arr2+1);
  printf("The addr of arr3 is %p and the addr of arr3+1 is %p\n",arr3,arr3+1);

  
  return 0;
}
