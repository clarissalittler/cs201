#include <stdio.h>
#include <stdlib.h>

struct Thingy {
  int thing1;
  double thing2;
};

int main(){
  printf("The size of a char is: %ld\n",sizeof(char));
  printf("The size of an int is: %ld\n",sizeof(int));
  printf("The size of a float is: %ld\n",sizeof(float));
  printf("The size of a double is: %ld\n",sizeof(double));
  printf("The size of a long is: %ld\n",sizeof(long));
  printf("The size of a thing is: %ld\n",sizeof(struct Thingy));
  printf("The size of a pointer is: %ld\n",sizeof(int*));
  
  return 0;
}
