#include <stdio.h>
#include <stdlib.h>

int main(){
  float f = 0;

  for(int i = 0; i < 100; i++){
    f += 0.01;
  }

  printf("Our number is %.40f\n",f);
  return 0;
}
