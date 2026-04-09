#include <stdio.h>
#include <stdlib.h>

int main(){

  float f = 0.0001;
  float accum = 0;
  
  for(int i=0; i<10000; i++){
    accum += f;
  }

  printf("%.20f\n",accum);
  
  return 0;
}
