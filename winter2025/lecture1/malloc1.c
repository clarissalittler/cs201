#include <stdio.h>
#include <stdlib.h>

struct Point{
  int x;
  int y;
};

int main(){
  printf("The size of Point is: %lu\n",sizeof(struct Point));
  
  // malloc instead of new
  struct Point* pointy = malloc(sizeof(struct Point));

  (*pointy).x = 10;
  (*pointy).y = 20;

  printf("The field x is %d and y is %d\n",(*pointy).x,(*pointy).y);

  pointy->x = 50;
  pointy->y = 40;

  printf("The field x is %d and y is %d\n",pointy->x,pointy->y);

  free(pointy);

  pointy = 0;
  
  return 0;
}
