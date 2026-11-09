// The FILE API, part 1: fopen and fclose
//
// open/read/write/close are system calls: they go straight to the kernel.
// fopen/fgets/fprintf/fclose are the C library's layer on top of them.
// You get a FILE* instead of an int, plus buffering and formatting.
//
// Modes for fopen:
//   "r"   read; the file must exist
//   "w"   write; creates the file, or ERASES it if it exists
//   "a"   append; creates the file, writes always go at the end
//   "r+"  read and write; the file must exist
//   add "b" ("rb", "wb") for binary -- does nothing on Linux, matters on Windows
//
// try:
//   ./stdioOpen sample.txt
//   ./stdioOpen nope.txt
//   ./stdioOpen /etc/shadow        (exists, but are you allowed to read it?)

#include <stdio.h>

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <file>\n", argv[0]);
    return 1;
  }

  FILE* f = fopen(argv[1], "r");
  if(f == NULL){          // fopen returns NULL on failure (open returned -1)
    perror(argv[1]);      // e.g. "nope.txt: No such file or directory"
    return 1;
  }

  char line[256];
  if(fgets(line, sizeof(line), f) != NULL){
    printf("first line of %s: %s", argv[1], line);   // line keeps its '\n'
  } else {
    printf("%s is empty\n", argv[1]);
  }

  fclose(f);              // flushes anything buffered, then closes the fd
  return 0;
}
