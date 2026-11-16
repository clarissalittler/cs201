// main.c + helper.c = prog
//
// main.c uses three things it doesn't define: bump, describe, and counter
// (plus printf from the C library). When we compile main.c by itself, the
// compiler leaves a hole wherever one of those is used, and writes a note
// (a "relocation") saying what belongs in the hole. The linker fills it in.
//
// try:
//   gcc -c main.c
//   nm main.o                        which symbols are U (undefined)?
//   objdump -dr main.o               the holes, with the relocation notes
//   ./reloc.sh main.o prog           the same code before and after linking
//   gcc main.o -o broken             what does the linker say is missing?

#include <stdio.h>
#include "helper.h"

int total;                    // B: global, zero-initialized
int start = 5;                // D: global, initialized
static int secret = 42;       // d: only main.c can see it

int main(void){
  total = bump(start);
  total = bump(secret);
  printf("counter = %d, total = %d (%s)\n", counter, total, describe(total));
  return 0;
}
