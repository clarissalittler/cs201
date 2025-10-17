#include "stdio.h"

int counter = 0;

int badRec(int n){
  int stuff = 1;
  int moreStuff = 2;
  counter++;
  if(n == 0){
    return 0;
  }
  else{
    printf("%d\n",counter);
    return badRec(n+1)+stuff+moreStuff;
  }
}

int main(){
  badRec(1000);

  return 0;
}
