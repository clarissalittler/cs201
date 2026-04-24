#include <stdio.h>
#include <stdlib.h>

int main(){
  int n = 0x89ABCDEF;

  unsigned char* c = (unsigned char*) &n;

  for(size_t i = 0; i < sizeof(n); i++){
    printf("The %zu th byte of n is: %x \n",i,*(c+i));
  }
  
  return 0;
}
