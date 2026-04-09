#include <stdio.h>
#include <stdlib.h>

int main(){

  int n;
  long l;
  char c;
  float f;
  double d;
  int* p = &n;

  printf("The size of an int is: %ld\n",sizeof(n));
  printf("The size of a long is: %ld\n",sizeof(l));
  printf("The size of a char is: %ld\n",sizeof(c));
  printf("The size of a float is: %ld\n",sizeof(f));
  printf("The size of a double is: %ld\n",sizeof(d));
  printf("The size of a pointer is: %ld\n",sizeof(p));
  
  return 0;
}
