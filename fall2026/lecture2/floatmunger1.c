#include <stdio.h>
#include <stdlib.h>

void printBits(int* num1){
  for(int i=31; i>=0;i--){
    if(i == 30 || i == 22){
      printf(" ");
    }
    printf("%d",(*num1>>i)&1);
  }
  printf("\n");
}

void toggleBit(int* num,int place){
  *num = (*num) ^ (1 << place);
}

int main(){

  float num1 = 1;
  int choice;

  do {
    printf("The number %.15f as bits looks like\n",
	   num1);
    printBits((int*)&num1);
    printf("Enter a bit to flip: ");
    scanf("%d",&choice);
    toggleBit((int*)&num1,choice);
  }while (choice != -1);

  return 0;
}
