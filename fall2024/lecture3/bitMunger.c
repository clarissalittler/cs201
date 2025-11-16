#include <stdlib.h>
#include <stdio.h>


void toggle(int* target, int place){
  int shifty = 1 << place;
  *target = *target ^ shifty;
}

int isSet(int target, int place){
  return ((target >> place) & 1);
}

void printBits(int target){
  for(int i=31; i >=0; i--){
    printf("%d",isSet(target,i));
  }
}

int main(){

  int testNum = 0;
  toggle(&testNum,31);
  //toggle(&testNum,30);
  //toggle(&testNum,29);

  printf("The bits set in the number %d are:\n",testNum);
  printBits(testNum);
  printf("\n");
  
  return 0;
}
