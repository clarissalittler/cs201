#include <stdio.h>
#include <stdlib.h>

int main(){
  int arr1[5] = {0,1,2,3,4};
  double arr2[5] = {0,1,2,3,4};

  // %d base ten integers
  // %x base 16 integers
  // %o base 8 integers
  // %p prints pointers
  // let's look at the width of the pointers
  // and how they behave when we advance the array
  printf("The start of arr1 is: %p\n",arr1);
  printf("The start of arr2 is: %p\n",arr2);
  for(int i = 1; i < 5; i++){
    printf("The %dth element of arr1 is at: %p\n",i,arr1 +i);
    printf("The %dth element of arr2 is at: %p\n",i,arr2 +i);
  }
  
  return 0;
}
