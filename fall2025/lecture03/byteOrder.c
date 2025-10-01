#include "stdio.h"

int main(){

  int num = 0x1a2b3c4d;

  unsigned char* pty = (unsigned char*)&num;
  for(int i = 0; i<4; i++){
    printf("%x ",pty[i]);
  }
  printf("\n");
  
  return 0;
}
