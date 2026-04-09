#include <stdio.h>
#include <stdlib.h>

int main(){

  long int num = 0x0123456789abcdef;
  unsigned char* p = (unsigned char*)&num;

  printf("The bytes are stored in memory as: ");
  for(int i=0; i<8; i++){
    printf("%x ",*(p+i));
  }
  printf("\n");
  
  return 0;
}
