#include <stdio.h>

int main(){

  int num1;
  int num2;

  printf("Enter a number: ");
  scanf("%d",&num1);
  printf("Enter another number: ");
  scanf("%d",&num2);

  printf("So %d and %d added together make %d\n",num1,num2,num1+num2);
  
  return 0;
}
