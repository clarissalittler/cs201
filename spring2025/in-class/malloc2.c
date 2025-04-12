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
    (struct Garbage*) malloc(10 * sizeof(struct Garbage)); // make an array of 10 struct Garbage

  // from here it's just a normal array
  g[0].n1 = 10;
  g[0].n2 = 20;

  printf("g[0].n1 is %d\n",g[0].n1);
  printf("g[0].n2 is %d\n",g[0].n2);

  free(g);
  return 0;
}
