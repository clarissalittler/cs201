#include <stdio.h>
#include <stdlib.h>

int main(){

  int arr1[5];
  double arr2[5];
  char arr3[5];

  for(int i=0; i<5; i++){
    printf("Addr of arr1[%d] is %p \n", i,
	 arr1+i);
    printf("Addr of arr2[%d] is %p \n", i,
	 arr2+i);
    printf("Addr of arr3[%d] is %p \n", i,
	 arr3+i);
  }
  // ac + 4 =10*16 + 12 + 4 = 176 = 11*16 = b0
  
  return 0;
}
