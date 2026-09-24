#include <stdio.h>
#include <stdlib.h>

struct Goofus{
  int g1;
  int g2[10];
};

int main(){
  int* narr = malloc(100*sizeof(int));
  narr[2] = 5;
  printf("%d\n",narr[2]);

  struct Goofus* g = malloc(sizeof(struct Goofus));

  g->g1 = 5; // (*g).g1

  printf("%ld\n",sizeof(struct Goofus));
  
  free(narr);
  free(g);
  
  return 0;
}
