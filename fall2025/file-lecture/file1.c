#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// FILE* type
int main(int argc, char* argv[]){

  FILE* ourFile = fopen(argv[1],"r+");

  // now let's use fgets
  // fgets is a line reader
  char line[1024];
  while( fgets(line,1024,ourFile) != NULL){
    printf("%s",line);
  }
  
  return 0;
}
