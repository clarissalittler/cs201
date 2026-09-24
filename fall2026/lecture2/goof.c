#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

void printBits(int num1){
  printf("%d in bits is: ",num1);
  for(int i=31; i>=0;i--){
    printf("%d",(num1>>i)&1);
  }
  printf("\n");
}


int main(){

  printf("Upper limit %d\n", INT_MAX);
  printf("Lower limit %d\n", INT_MIN);

  char c1 = 101;
  int i1 = (int) c1;
  int i2 = (int) (-c1);
  
  printf("after casting c1 looks like...\n");
  printBits(i1);
  printf("after casting -c1 looks like...\n");
  printBits(i2);

  printf("UINT_MAX + 1 is...%d\n",UINT_MAX+1);
  printf("INT_MAX + 1 is...%d\n",INT_MAX+1);
  
  return 0;
}
