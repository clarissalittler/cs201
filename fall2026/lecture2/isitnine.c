#include <stdio.h>
#include <stdlib.h>

int main(){
  printf("is (3 << 2) ^ 5 = 9???: %d\n",
	 ((3 << 2) ^ 5) == 9);
  return 0;
}
