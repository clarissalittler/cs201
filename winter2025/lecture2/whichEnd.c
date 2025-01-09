#include <stdio.h>
#include <stdlib.h>

// are we big endian?
// or little endian?

int main(){
  // every 2 hex digits is an 8-bit byte
  int num = 0x12345678;

  unsigned char* pointy = (unsigned char*)&num;

  printf("Our number's bytes, in order, are: ");
  for(int i = 0; i < sizeof(int); i++){
    printf(" %x ",*(pointy+i));
  }
  printf("\n");
  return 0;
}
