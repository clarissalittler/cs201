// The smallest C program, so we can watch every stage of the build.
//
// try:
//   ./pipeline.sh hello.c            preprocess, compile, assemble, link
//   ./peek.sh hello                  what's inside the finished program?
//   xxd -l 64 hello                  the ELF header, byte by byte
//   ./patch-string.sh hello "pipeline" "linker!!"
//
// The string lives in the .rodata section of the executable. patch-string.sh
// finds it and overwrites it in a copy of the program.

#include <stdio.h>

#define GREETING "Hello from the pipeline!"

int main(void){
  printf("%s\n", GREETING);
  return 0;
}
