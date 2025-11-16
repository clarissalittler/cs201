#include <stdlib.h>
#include <stdio.h>


void toggle(int* target, int place){
  int shifty = 1 << place;
  *target = *target ^ shifty;
}

int isSet(int* target, int place){
  return ((*target >> place) & 1);
}

void printBits(int* target){
  for(int i=31; i >=0; i--){
    printf("%d",isSet(target,i));
  }
}

int main(){
  /*
  float testNum = 0.3333;
  toggle((int*)&testNum,12);
  toggle((int*)&testNum,30);
  toggle((int*)&testNum,24);*/

  float targetNum = 0.1;
  float testNum = 0;

  for(int i =0; i < 10; i++){
    testNum = testNum+0.01;
  }
  /*
  toggle((int*)&testNum,31);
  toggle((int*)&testNum,30);
  toggle((int*)&testNum,29);
  toggle((int*)&testNum,28);
  toggle((int*)&testNum,27);
  toggle((int*)&testNum,26);
  toggle((int*)&testNum,25);
  toggle((int*)&testNum,24);
  toggle((int*)&testNum,23);

  toggle((int*)&testNum,0);
  */
  printf("The bits set in the number %.12f are:\n",targetNum);
  printBits((int*) &targetNum);
  printf("\n");

  printf("The bits set in the number %.12f are:\n",testNum);
  printBits((int*) &testNum);
  printf("\n");

  
  return 0;
}
