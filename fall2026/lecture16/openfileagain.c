// That was the basic system call structure
// which, as I cleverly demonstrated, has a lot of pieces
// that are easy to make annoying mistakes on
// on the other hand, we have the actual FILE api
// that comes from the higher-level interface of C's standard library

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){

  FILE* f = fopen("test.txt", "r");
  if(f == NULL){
    printf("Couldn't open test.txt\n");
    return 1;
  }
  char buf[100];

  // fgets returns a pointer (or NULL at EOF/error), so compare to NULL
  while(fgets(buf, sizeof(buf),f) != NULL){
    printf("%s",buf);
  }
  
  
  
  return 0;
}
