#include <stdio.h>
#include <stdlib.h>

int main(){

  float f1 = 0.00001;
  float accum = 0;
  for(int i=0; i<100000; i++){
    accum += f1;
  }

  printf("The accumulator is...%.10f\n",accum);
  
  return 0;
}
