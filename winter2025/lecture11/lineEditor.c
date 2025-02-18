#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


const int linesize = 1024;

void cleanUp(FILE* f, char** ls, int nL){
  ftruncate(fileno(f),0);
  fseek(f, 0, SEEK_SET);
  for(int i=0; i < nL; i++){
    fputs(ls[i],f);
    free(ls[i]);
  }
  free(ls);
  fclose(f);
}

void printFile(char** ls, int nL){
  printf("The state of the file is: \n");
  for(int i =0; i < nL; i++){
    printf("%d: %s", i, ls[i]);
  }
}

void insLine(int line, char** ls, int* nL){

  char* newLine = malloc(linesize*sizeof(char));
  char c;
  printf("New text to insert at line %d:\n",line);

  while ((c = getchar()) != '\n' && c != EOF); // trick for clearing the input buffer

  fgets(newLine, linesize, stdin);
  for(int i = *nL; i > line; i--){
    ls[i] = ls[i-1];
  }
  ls[line] = newLine;
  (*nL)++;
}

void delLine(int line, char** ls, int* nL){
  char* oldPointer = ls[line];
  for(int i = line; i < (*nL)-1; i++){
    ls[i] = ls[i+1];
  }
  free(oldPointer);
  ls[(*nL)-1] = NULL;
  (*nL)--;
}

void editLine(int line, char** ls, int nL){
  printf("Type the text you want to replace line %d:\n",line);
  char c;
  
  while ((c = getchar()) != '\n' && c != EOF);
  
  char *newLine = malloc(linesize*sizeof(char));
  fgets(newLine, linesize, stdin);

  free(ls[line]);
  
  ls[line] = newLine;
}

int main(int argc, char* argv[]){

  FILE *ourFile = fopen(argv[1],"r+");

  if(ourFile < 0){
    return 1;
  }
  
  char** lines = malloc(10000*sizeof(char*));
  int numLines = 0;

  lines[0] = malloc(linesize);
  while(fgets(lines[numLines],linesize,ourFile)){
    numLines++;
    lines[numLines] = malloc(linesize);
  }
  //free(lines[numLines]);
  printf("The file you're going to edit is: \n");
  for(int i = 0; i < numLines; i++){
    printf("line %d: %s",i,lines[i]);
  }

  int option = 0;
  while(1){
    printf("Welcome to our goofy file editor:\n");
    printFile(lines,numLines);
    printf("\n0: edit a line\n");
    printf("1: delete a line\n");
    printf("2: insert a line\n");
    printf("3: quit\n");

    scanf("%d",&option);
    switch(option){
      int line;
    case 0:
      printf("While line to edit?: ");
      scanf("%d",&line);
      editLine(line, lines, numLines);
      break;
    case 1:
      printf("Which line to delete?: ");
      scanf("%d",&line);
      delLine(line,lines,&numLines);
      break;
    case 2:
      printf("Which line to insert?: ");
      scanf("%d",&line);
      insLine(line,lines,&numLines);
      break;
    case 3:
      printf("Goodbye!\n");
      cleanUp(ourFile,lines,numLines);
      return 0;
    }
  }
  
  return 0;
}
