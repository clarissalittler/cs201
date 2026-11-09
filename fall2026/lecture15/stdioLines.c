// The FILE API, part 2: reading line by line with fgets
//
// fgets(buf, size, f) reads up to and including the next '\n',
// but never more than size-1 characters, and always adds a '\0'.
// It returns NULL at end of file (or on an error).
//
// This is "cat -n": print every line with a line number.
//
// try:
//   ./stdioLines sample.txt
//   then change LINE_SIZE to 16, rebuild, and run it again.
//   The output looks the same -- but how many fgets calls did it take?
//   What happens to a line that doesn't fit?

#include <stdio.h>
#include <string.h>

#define LINE_SIZE 256

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <file>\n", argv[0]);
    return 1;
  }

  FILE* f = fopen(argv[1], "r");
  if(f == NULL){
    perror(argv[1]);
    return 1;
  }

  char line[LINE_SIZE];
  int lineNum = 1;
  int calls = 0;
  int atStart = 1;     // are we at the beginning of a line?
  while(fgets(line, sizeof(line), f) != NULL){
    calls++;
    if(atStart){
      printf("%4d  ", lineNum);
    }
    printf("%s", line);

    // if the line was too long for our buffer, fgets stops early and
    // there's no '\n' yet -- the rest of the line comes on the next call
    if(strchr(line, '\n') != NULL){
      lineNum++;
      atStart = 1;
    } else {
      atStart = 0;
    }
  }

  fclose(f);
  fprintf(stderr, "(%d lines, %d calls to fgets)\n", lineNum - 1, calls);
  return 0;
}
