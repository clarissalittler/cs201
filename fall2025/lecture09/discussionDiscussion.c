/*

With your partner, write a C program that lets you read in two numbers and then add them together. Compile this code and check to make sure it works. Then generate assembly code from this using the -S option, find the instructions where the numbers are added together, and instead change it to a multiplication in the assembly.

Check this program compiles and upload your program in a post! Try also changing your prompt in assembly as well.
  
*/

#include "stdio.h"

int main(){

  int num1; // -8(%rbp)
  int num2; // -12(%rbp)

  printf("Enter a number: ");
  scanf("%d",&num1);
  printf("Enter another: ");
  scanf("%d",&num2);

  int num3 = num1 + num2; // num3 is -4(%rbp)

  printf("Ta-da! %d\n",num3);

  return 0;
}
