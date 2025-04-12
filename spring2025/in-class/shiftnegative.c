#include <stdlib.h>
#include <stdio.h>

void printBits(int n){
  for(int i = 31; i >= 0; i--){
    printf("%d",(n >> i) & 1);
  }
  printf("\n");
}

void flipBit(int* n, int c){
  *n = (*n) ^ (1 << c);
}


int main(){
  unsigned int num = -1; // 1111111..11111
  printf("Our number before shifting is: %d\n",num);
  printBits(num);
  num = num >> 3;
   printf("Our number after shifting is: %d\n",num);
  printBits(num);
  return 0;
}
