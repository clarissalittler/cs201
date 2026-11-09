// The FILE API, part 3: one character at a time with fgetc
//
// This is a tiny version of wc: count characters, words, and lines.
//
// Note that stdin is a FILE* too (so are stdout and stderr), so
// with no filename we just read from stdin instead.
//
// try:
//   ./stdioChars sample.txt
//   wc sample.txt                      (do we agree?)
//   ls -l | ./stdioChars
//   ./stdioChars                       (type some lines, then ctrl-D for EOF)

#include <stdio.h>
#include <ctype.h>

int main(int argc, char* argv[]){
  FILE* f = stdin;
  if(argc >= 2){
    f = fopen(argv[1], "r");
    if(f == NULL){
      perror(argv[1]);
      return 1;
    }
  }

  long chars = 0, words = 0, lines = 0;
  int inWord = 0;

  // c is an int, not a char! fgetc returns any byte value 0-255
  // OR the special value EOF (-1), so it needs room for 257 possibilities.
  int c;
  while((c = fgetc(f)) != EOF){
    chars++;
    if(c == '\n') lines++;

    if(isspace(c)){
      inWord = 0;
    } else if(!inWord){
      inWord = 1;
      words++;
    }
  }

  printf("%ld lines, %ld words, %ld chars\n", lines, words, chars);

  if(f != stdin) fclose(f);
  return 0;
}
