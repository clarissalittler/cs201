#include <stdio.h>

struct point {
  double x;
  double y;
};

double magSq(struct point p){
  return (p.x*p.x + p.y*p.y);
}

int main(){

  struct point p1;
  p1.x = 5.0;
  p1.y = 10;

  double* dubstep = &p1;
  printf("This is (hopefully) p1.x %f\n",*dubstep);
  printf("This is (hopefully) p1.y %f\n",*(dubstep + 1));
  
  printf("The d^2 of this vector is %f\n",magSq(p1));
  
  return 0;
}
