#include <stdio.h>
#include <stdlib.h>

const int linesize = 1024;

int main(int argc,char* argv[]){

  FILE *ourFile = fopen(argv[1],"rw");

  char** lines = malloc(10000*sizeof(char*)); // setting aside room 10k lines of text
  // obviously we'd want to do something different for a bigger document but you wouldn't use a line editor for that anyway lbr
  int numLines = 0;
  lines[0] = malloc(linesize);
  while(fgets(lines[numLines],linesize,ourFile)){
    numLines++;
    lines[numLines] = malloc(linesize);
  }
  // huh is there technically a problem here?? let's come back to it

  // now let's first make sure we can print all of this out correctly
  for(int i = 0; i < numLines; i++){
    printf("line %d: %s",i,lines[i]);
  }
  // better free it
  for(int i = 0; i < numLines; i++){
    free(lines[i]);
  }
  //yeah, neat
  free(lines);
  fclose(ourFile);
  return 0;
}
