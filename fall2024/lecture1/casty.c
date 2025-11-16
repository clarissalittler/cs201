#include <stdio.h>
#include <stdlib.h>

int main(){

  int ourInt = -10;
  unsigned int otherInt = (unsigned int)ourInt;

  printf("As an int %d as an unsigned %u\n", ourInt, otherInt);
  
  return 0;
}
