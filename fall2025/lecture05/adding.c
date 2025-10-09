#include "stdio.h"

int adder(int n1, int n2){
  int res;
  res  = n1 + n2;
  return res;
}

int main(){
  int n1 = 10;
  int n2 = 20;
  printf("%d\n",adder(n1,n2));

  return 0;
}
