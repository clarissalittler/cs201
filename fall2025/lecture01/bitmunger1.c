#include "stdio.h"
#include "stdlib.h"

// & bitwise and
// | bitwise or
// ^ bitwise xor
//
// 1 ^ 1 = 0
// 0 ^ 1 = 1
// 1 ^ 0 = 1
// 0 ^ 0 = 0
// num1 << num2 move all the bits in num1 to the left num2 places
// num1 >> num2 move all the bits in num1 to the right num2 places



void printBits(int num1){
  printf("%d in bits is: ",num1);
  for(int i=31; i>=0;i--){
    printf("%d",(num1>>i)&1);
  }
  printf("\n");
}

void toggleBit(int* num,int place){
  *num = (*num) ^ (1 << place);
}

int main(){

  int num1 = 80;
  int choice;

  do {
    printf("The number %d when interpreted as unsigned is: %u\n",num1,(unsigned int)num1);
    printBits(num1);
    printf("Enter a bit to flip: ");
    scanf("%d",&choice);
    toggleBit(&num1,choice);
  }while (choice != -1);

  return 0;
}
