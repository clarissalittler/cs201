#include <stdio.h>
#include <stdlib.h>

// memory access is done on boundaries that are multiples of the architecture size
// 64-bit will be on 8-byte boundaries
// structs may pad fields to keep alignment properties
struct Goofy1 {
  int num1;
  int num2;
};

struct Goofy2 {
  int num1;
  double num2;
};

struct Goofy3 {
  double num1;
  int num2;
};

int main(){

  struct Goofy1 g1;
  struct Goofy2 g2;
  struct Goofy3 g3;
  
  printf("Goofy1 size: %ld\n",sizeof(g1));
  printf("Goofy2 size: %ld\n",sizeof(g2));
  printf("Goofy3 size: %ld\n",sizeof(g3));
  
  return 0;
}
