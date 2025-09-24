#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
// no using namespace std;

struct Rect {
  int width;
  int height;
};

int main(){

  int* arr1 = malloc(100*sizeof(int));

  for(int i=0; i<100; i++){
    *(arr1 + i) = i*i;
    printf("The %d th element is %d\n",i,arr1[i]);
  }
  
  free(arr1);
  return 0;
}
