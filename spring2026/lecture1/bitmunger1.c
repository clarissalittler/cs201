#include "stdio.h"
#include "stdlib.h"

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

void twosComplement(int* num){
    *num = ~((*num) - 1);
}

int main(){

  int num1 = 4;
  int choice;

  do {
    printf("The number %d when interpreted as unsigned is: %u\n",
	   num1,(unsigned int)num1);
    printBits(num1);
    printf("Enter a bit to flip: ");
    scanf("%d",&choice);
    toggleBit(&num1,choice);

    printf("Invert?: ");
    scanf("%d",&choice);
    if(choice){
      twosComplement(&num1);
    }
  }while (choice != -1);

  return 0;
}
