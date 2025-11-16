#include <stdio.h>

int main(){

  float test = 0;
  for(int i = 0; i < 10; i++){
    test = test + 0.01;
  }

  if(test == 0.1){
    printf("Everything's good\n");
  }
  else{
    printf("Whoops\n");
    printf("Test is actually: %f\n",test);
  }
  
  return 0;
}
