#include <stdio.h>
#include <stdlib.h>

const int linesize = 1024;

void insLine(int line, char** ls, int* nL){

  char* newLine = malloc(1024);
  char c;
  printf("New text to insert at line %d:\n",line);

  while ((c = getchar()) != '\n' && c != EOF); // trick for clearing the input buffer

  fgets(newLine, 1024, stdin);


  for(int i = *nL; i > line; i--){
    ls[i] = ls[i-1];
  }
  ls[line] = newLine;
  (*nL)++;
}

void editLine(int line, char** ls, int nL){
}

void delLine(int line, char** ls, int* nL){
}

void cleanUp(FILE* f, char** ls, int nL){
  // need to write the whole thing back to the file
  // TODO
  fseek(f, 0, SEEK_SET);
  for(int i = 0; i < nL; i++){
    fputs(ls[i],f);
    free(ls[i]);
  }
  free(ls);
  fclose(f);
}

int main(int argc,char* argv[]){

  FILE *ourFile = fopen(argv[1],"r+");

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

  //now let's create our main loop with a menu
  int option = 0;
  while(1){
    printf("Welcome to our goofy file editor:\n");
    printf("0: edit a line\n");
    printf("1: delete a line\n");
    printf("2: insert a line\n");
    printf("3: quit\n");

    scanf("%d",&option);
    switch (option) {
      int line;
      case 0:
        printf("Which line to edit?: ");
        scanf("%d",&line);
        editLine(line,lines,numLines);
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
        printf("Goodbye!");
        cleanUp(ourFile,lines,numLines);
        return 0;
    }
  }

  return 0;
}
