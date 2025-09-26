#include "stdio.h"
#include "stdlib.h"

struct Goofus {
  int arr[100];
};

int main(){

  struct Goofus* g = malloc(sizeof(struct Goofus));

  printf("A goofus is %ld bytes big\n",sizeof(struct Goofus));

  free(g);
  
  return 0;
}
