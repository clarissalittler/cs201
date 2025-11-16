#include <stdio.h>
#include <stdlib.h>

/* 
 we want a program that will traverse
 an integer byte by byte 
 by casting a pointer to the integer
 to a one byte wide pointer type
*/

int main(){

  int ourNum = 0x12345678;

  //printf("ourNum = %d\n",ourNum);
  //printf("ourNum = %u\n",ourNum);
  
  unsigned char* pointy = (unsigned char*) &ourNum;
  //printf("The first byte of our number is: %x\n", pointy[3]);

  //the bytes in memory order of ourNum
  for(int i=0; i < sizeof(typeof(ourNum)); i++){
    printf("byte #%d = %x\n",i,pointy[i]);
  }
  
  return 0;
}
