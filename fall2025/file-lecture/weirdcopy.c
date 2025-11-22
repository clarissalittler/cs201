#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// this uses FILE* instead
int main(int argc, char* argv[]){

  FILE* inputFile = fopen(argv[1],"r+");
  FILE* outputFile = fopen(argv[2],"w");
  
  // now let's use fgets
  // fgets is a line reader
  char line[1024];
  while( fgets(line,1024,inputFile) != NULL){
    fputs(line,outputFile);
  }

  fclose(inputFile);
  fclose(outputFile);
  
  return 0;
}
