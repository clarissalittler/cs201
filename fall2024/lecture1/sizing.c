#include <stdio.h>
#include <stdlib.h>

int main(){

  char c;
  int i;
  float f;
  double nd;
  int* p1;
  double** p2;

  printf("The size of a char is: %zu\n",sizeof(c));
  printf("The size of an int is: %zu\n",sizeof(i));
  printf("The size of a float is: %zu\n",sizeof(f));
  printf("The size of a double is: %zu\n",sizeof(nd));
  printf("The size of an int* is: %zu\n",sizeof(p1));
  printf("The size of a double** is: %zu\n",sizeof(p2));
  
  return 0;
}
