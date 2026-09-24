#include <stdio.h>
#include <stdlib.h>

int main(){

  int intArr[5];
  char charArr[5];
  char* pointArr[5];

  for(int i=0; i<5; i++){
    printf("the %d slot of intArr is at %p\n",i,intArr+i);
    printf("the %d slot of charArr is at %p\n",i,charArr+i);
    printf("the %d slot of pointArr is at %p\n",i,pointArr+i); 
  }
  
  return 0;
}
