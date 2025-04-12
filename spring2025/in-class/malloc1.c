#include <stdio.h>
#include <stdlib.h>

// malloc
// takes a size in bytes
// and then gives back a pointer
// to a region of that size!
// not like new!
// it's your responsibility to make sure
// you cast it to the correct type!!

struct Garbage {
  int n1;
  int n2;
};

int main(){
  struct Garbage* g =
    (struct Garbage*) malloc(sizeof(struct Garbage));

  g->n1 = 10;
  g->n2 = 20;

  printf("g->n1 is %d\n",g->n1);
  printf("g->n2 is %d\n",g->n2);

  return 0;
}
