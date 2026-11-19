#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  FILE* output = stdout;

  char line[64] = "This is a thing\n";

  fputs(line,output);
  fclose(output);
  
  return 0;
}
