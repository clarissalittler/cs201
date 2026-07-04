#include <stdio.h>
#include <stdlib.h>

void printBits(int* num1){
  printf("in bits is: ");
  for(int i=31; i>=0;i--){
    printf("%d",(*num1>>i)&1);
    if(i == 31 || i == 23){
      printf(" ");
    }
  }
  printf("\n");
}

void toggleBit(int* num,int place){
  *num = (*num) ^ (1 << place);
}

int main(){

  float num1 = 2;
  int choice;

  do {
    printf("The number %.16f ",num1);
    printBits((int*)&num1);
    printf("Enter a bit to flip: ");
    scanf("%d",&choice);
    toggleBit((int*)&num1,choice);

  }while (choice != -1);

  return 0;
}
