#include "stdio.h"
#include "stdlib.h"

struct Rect {
  int width;
  int height;
};

int main(){

  struct Rect* r = malloc(sizeof(struct Rect));

  r->width = 10; // (*r).width
  r->height = 20;

  printf("%d\n", r->width * r->height);

  free(r);

  return 0;
}
