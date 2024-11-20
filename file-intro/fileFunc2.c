#include <stdio.h>
#include <stdlib.h>

// demonstrate how to open a file
// with the normal libraries

// now we want to read in the file all at once
// and store it in memory instead of printing it
// as we traverse

int main(int argc, char* argv[]){

  const int lineSize = 1024;
  const int fileSize = 10000; // our current limit
  
  FILE* ourFile = fopen(argv[1],"r");
  char** lines = malloc(fileSize * sizeof(char*));  
  int numLines = 0;
  
  // char* fgets(char*,size_t,FILE*)
  // Result<string,IOError>
  // Option<string>
  lines[0] = malloc(lineSize * sizeof(char));
  while(fgets(lines[numLines],lineSize,ourFile)){
    numLines++;
    lines[numLines] = malloc(lineSize * sizeof(char));
  }
  // TODO put the cleanup for the last attempted line
  free(lines[numLines]);

  // here's our working code
  for(int i = 0; i < numLines; i++){
    printf("%s",lines[i]);
  }
  
  // now I have to clean up after myself!!
  for(int i = 0; i < numLines; i++){
    free(lines[i]);
  }

  free(lines);
  fclose(ourFile);
  
  return 0;
}
