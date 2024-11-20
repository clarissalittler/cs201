#include <stdio.h>

int main(int argc, char* argv[]){

  FILE *ourFile = fopen(argv[1],"r");

  char line[2048];

  char* ptr = fgets(line, sizeof(line), ourFile);
  while (ptr != NULL){
    printf("%s", line);
    ptr = fgets(line, sizeof(line), ourFile);
  }


  return 0;
}
