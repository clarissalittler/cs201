#include "stdio.h"
#include "stdlib.h"

int main(){

  double num = 0.00001;
  double acc = 0;

  
  for(int i = 0; i < 100000; i++){
    acc = acc + num;
  }

  if(acc != 1){
    printf("UH OH %f is not equal to 1\n",acc);
  }
  if((acc < 1 + 0.000001) && acc > (1 - 0.000001)){
    printf("But it's close enough!\n");
  }

  return 0;
}
