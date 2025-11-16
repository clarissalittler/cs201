#include <stdio.h>

int goofyfunction(int a, int b){
  return (a * b + a);
}

int main(){
  char* msg = "This is silly\n";

  int num1 = 5;
  int num2 = 6;

  int num3 = goofyfunction(num1,num2);

  printf("%s %d",msg,num3);
  
  return 0;
}
