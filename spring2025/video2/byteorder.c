#include <stdio.h>
#include <stdlib.h>

int main(){
  int n = 0x89ABCDEF;

  unsigned char* c = (unsigned char*) &n;

  for(int i=0; i<sizeof(typeof(n)); i++){
    printf("The %d th byte of n is: %x \n",i,*(c+i));
  }
  
  return 0;
}
