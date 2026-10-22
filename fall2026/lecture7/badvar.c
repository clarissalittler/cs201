#include <stdio.h>
#include <stdlib.h>

int* badfun(){
  int x = 10;
  return &x;
}

int main(){

  int n = *badfun();

  printf("The value of n is: %d\n",n);
  
  return 0;
}
