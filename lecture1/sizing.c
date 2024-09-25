#include <stdio.h>
#include <stdlib.h>

int main(){

  char c;
  int i;
  float f;
  double nd;
  int* p1;
  double** p2;

  printf("The size of a char is: %u\n",sizeof(c));
  printf("The size of an int is: %u\n",sizeof(i));
  printf("The size of a float is: %u\n",sizeof(f));
  printf("The size of a double is: %u\n",sizeof(nd));
  printf("The size of an int* is: %u\n",sizeof(p1));
  printf("The size of a double** is: %u\n",sizeof(p2));
  
  return 0;
}
