#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){

  printf("There are %d arguments and they are: \n",argc);
  for(int i=0; i<argc; i++){
    printf("Argument %d is %s\n",i,argv[i]);
  }
  
  return 0;
}
