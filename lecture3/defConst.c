#include <stdio.h>
#include <stdlib.h>

const int thing = 10;

#define thing2 20

struct point {
  double x;
  double y;
};

const struct point p = {20, 30};

// #define p2 = {20,30}
#define msg "Hi there"

/*
int myMax(int x, int y){
  return (x > y ? x : y);
}
*/

#define myMax(x,y) (x > y ? x : y)

#define badSizeof(x) 1+sizeof(x)
#define size2(x) sizeof(typeof(x))

int main(){

  //thing = 0xFFFFFFFF;
  //printf("Here's a thing: %d\n",myMax("10",20));
  printf("Here's a thing: %d\n",size2(5.0));
  
  return 0;
}
