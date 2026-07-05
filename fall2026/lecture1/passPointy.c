#include <stdio.h>

void intMunger(int* p){
  *p = 2*(*p);
}

int main(){

  int test;

  printf("Enter an int, will ya: ");
  
  scanf("%d",&test);

  intMunger(&test);

  printf("%d\n",test);
  
  return 0;
}
