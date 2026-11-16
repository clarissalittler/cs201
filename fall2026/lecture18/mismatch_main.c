// A bug the linker used to let through.
//
// mismatch_main.c says `int x;`. mismatch_other.c says `double x;`. Neither
// one has an initializer, so neither looks like "the" definition. There's
// no shared header, so nobody ever compares the two types.
//
// try:
//   gcc mismatch_main.c mismatch_other.c -o mismatch
//       Modern gcc (10 and later) refuses: multiple definition of `x'.
//
//   gcc -fcommon mismatch_main.c mismatch_other.c -o mismatch
//   ./mismatch
//       The old behavior. The linker quietly merges the two x's into one
//       8-byte slot (no warning!), and set_x() stores a double there. We
//       read back the first 4 bytes of 3.14 as if they were an int.
//
//   nm -S mismatch | grep -w x       one x, and its size is 8, not 4
//
// The real fix: one header with `extern int x;`, included by both files,
// so the compiler can see the conflict.

#include <stdio.h>

int x;

void set_x(void);

int main(void){
  x = 1;
  printf("before set_x: x = %d\n", x);
  set_x();
  printf("after set_x:  x = %d (%#x)\n", x, x);
  return 0;
}
