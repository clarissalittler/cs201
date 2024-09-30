#include <stdlib.h>
#include <stdio.h>

// this function toggles the nth bit in a number
void toggle(int* target, int place){
  // you should set the place-th bit in target
  // you can assume it will always be < 32
  int shifty = 1 << place;
  *target = *target ^ shifty;
}

int isSet(int target, int place){
  // this should return 0 if the place-th bit is 0
  // and return 1 if it is 1
  return ((target >> place) & 1);
}

int main(){

  int testNum = 0;
  //toggle(&testNum,31);
  toggle(&testNum,30);
  toggle(&testNum,29);

  printf("The bits set in the number %d are:\n",testNum);

  for(int i=31; i >=0; i--){
    printf("%d",isSet(testNum,i));
  }
  printf("\n");
  /*
  int num1 = 65;
  int num2 = 0xFFFF;

  if(isSet(num1,0)){
    printf("Passed test 1\n");
  }
  toggle(&num2,16);
  if(num2 > 0xFFFF){
    printf("Passed test 2\n");
  }
  */
  
  return 0;
}
