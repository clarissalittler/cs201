#include "stdio.h"

/*
  hex    / decimal 
  0        0
  1        1
  2        2
  3        3
  4        4
  5        5
  6        6
  7        7
  8        8
  9        9 
  A        10
  B        11
  C        12
  D        13
  E        14
  F        15

  00   ->  0
  FF   ->  16*15 + 15 = 255
*/

int main(){

  int num;
  printf("The size of a char is: %ld\n",sizeof(char));
  
  char arr[10];

  for(int i = 0; i < 10; i++){
    printf("The address of the %d th element is %p\n",i,arr+i);
  }
  
  return 0;
}
