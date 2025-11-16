#include <stdio.h>
#include <stdlib.h>

int main(){
  int arr1[10];
  double arr2[10];
  
  for(int i = 0; i < 10; i++){
    arr1[i] = i*i;
    arr2[i] = i*3.14;
  }

  printf("arr1[3] = %d\n",arr1[3]);
  printf("arr1[3] = %d\n",*(arr1 + 3));

  printf("arr2[3] = %f\n",arr2[3]);
  printf("arr2[3] = %f\n",*(arr2 + 3));
  
  return 0;
}
