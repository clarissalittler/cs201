#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


const int linesize = 1024;

void cleanUp(FILE* f, char** ls, int nL){
  // wipe it out
  ftruncate(fileno(f),0);
  // just to be sure, we make sure we're seeked to the 0 position
  fseek(f, 0, SEEK_SET);
  // for each line we print the line and then free it
  for(int i=0; i < nL; i++){
    fputs(ls[i],f);
    free(ls[i]);
  }
  // free the collection of lines
  free(ls);
  // close it up!
  fclose(f);
}

void printFile(char** ls, int nL){
  printf("The state of the file is: \n");
  for(int i =0; i < nL; i++){
    printf("%d: %s", i, ls[i]);
  }
}

void insLine(int line, char** ls, int* nL){

  // well we need to allocate a new line in order to
  // have something to insert
  char* newLine = malloc(linesize*sizeof(char));
  int c; // int, not char: getchar() returns an int so EOF (-1) stays distinguishable from byte 0xFF
  printf("New text to insert at line %d:\n",line);

  while ((c = getchar()) != '\n' && c != EOF); // trick for clearing the input buffer
  // okay it's kinda weird that we're using getchar but then fgets with stdin, eh?
  // well the reason why is that you're basically never supposed to use gets
  // because it can't respect anything about buffer size
  // so you use fgets *with* stdin *and* the argument of how big the buffer is
  fgets(newLine, linesize, stdin);
  // we need to shift the whole array to the right
  // down to the point of where the new line should go
  // that's why this is a downwards for-loop
  for(int i = *nL; i > line; i--){
    ls[i] = ls[i-1];
  }
  // place in the new line by just changing the pointer 
  ls[line] = newLine;
  (*nL)++;
}

void delLine(int line, char** ls, int* nL){
  char* oldPointer = ls[line];
  // now what we do is we collapse with a leftward shift
  // thus overwriting the old pointers
  for(int i = line; i < (*nL)-1; i++){
    ls[i] = ls[i+1];
  }
  // now we're free to, uhh, free this pointer
  free(oldPointer);
  // for GOOD MEASURE make sure we take the old end of the array
  // and set it to NULL
  ls[(*nL)-1] = NULL;
  (*nL)--;
}

void editLine(int line, char** ls, int nL){
  (void)nL;
  printf("Type the text you want to replace line %d:\n",line);
  int c; // int, not char: getchar() returns an int so EOF (-1) stays distinguishable from byte 0xFF

  // clear everything from input before we do the fgets
  while ((c = getchar()) != '\n' && c != EOF);
  
  char *newLine = malloc(linesize*sizeof(char));
  fgets(newLine, linesize, stdin);

  free(ls[line]); // free the old text
  
  ls[line] = newLine; // change the pointer to be the new text
}

int main(int argc, char* argv[]){
  (void)argc;

  // we need to open the supplied filename
  // with the r+ option
  // because we want to read in the file
  // but *also* save out the file when we close our program

  FILE *ourFile = fopen(argv[1],"r+");

  if(ourFile == NULL){
    return 1;
  }

  // we're kinda inefficient here and we set aside space
  // for 10000 lines (which is really 10000 ptrs)
  
  char** lines = malloc(10000*sizeof(char*));
  // we'll keep track of the number of lines actually existent
  int numLines = 0;

  // we assume at least one line
  // note to self: can I simplify by just inverting the order
  // of the numLines and the lines[numLines] = malloc(linesize)?
  // I think I can
  lines[0] = malloc(linesize);
  while(fgets(lines[numLines],linesize,ourFile)){
    numLines++;
    lines[numLines] = malloc(linesize);
  }
  // we're just allocating space for all the lines based on
  // the data we read in

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
      printf("Which line number to edit?: ");
      scanf("%d",&line);
      editLine(line, lines, numLines);
      break;
    case 1:
      printf("Which line number to delete?: ");
      scanf("%d",&line);
      delLine(line,lines,&numLines);
      break;
    case 2:
      printf("Which line number to insert before?: ");
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
