#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

  int lineSize = 1024;
  int fileSize = 10000;

  FILE* ourFile = fopen(argv[1],"r");
  char** lines = malloc(fileSize * sizeof(char*));
  int numLines = 0;

  lines[0] = malloc(lineSize * sizeof(char));
  while(fgets(lines[numLines],lineSize,ourFile)){
    numLines++;
    lines[numLines] = malloc(lineSize * sizeof(char));
  }
  free(lines[numLines]);

  for(int i = 0; i < numLines; i++){
    printf("%s",lines[i]);
  }

  for(int i = 0; i < numLines; i++){
    free(lines[i]);
  }
  free(lines);
  fclose(ourFile);
  
  return 0;
}
