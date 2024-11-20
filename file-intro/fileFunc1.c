#include <stdio.h>
#include <stdlib.h>

// demonstrate how to open a file
// with the normal libraries

int main(int argc, char* argv[]){

  const int lineSize = 1024;
  
  FILE* ourFile = fopen(argv[1],"r");

  char line[lineSize];

  // char* fgets(char*,size_t,FILE*)
  // Result<string,IOError>
  // Option<string>
  while(fgets(line,lineSize,ourFile)){
    printf("%s",line);
  }
  
  fclose(ourFile);
  
  return 0;
}
