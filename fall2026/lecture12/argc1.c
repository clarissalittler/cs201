#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

  // we gonna play here

  for(int i=0; i<argc; i++){
    printf("The %d th argument is %s\n", i, argv[i]);
  }
  
  return 0;
}
