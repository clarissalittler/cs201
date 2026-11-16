// Two files, one name.
//
// dup_a.c and dup_b.c both define a global called shared. Each file compiles
// fine by itself, because the compiler only ever sees one file at a time.
//
// try:
//   gcc dup_a.c dup_b.c -o dup       multiple definition of `shared'
//   nm dup_a.o dup_b.o               (after gcc -c dup_a.c dup_b.c)
//
// Then make dup_b.c's copy private and link again:
//   sed 's/^int shared/static int shared/' dup_b.c > dup_b_static.c
//   gcc dup_a.c dup_b_static.c -o dup
//   ./dup
//   nm dup | grep shared             two of them now: one D, one d

#include <stdio.h>

int shared = 1;

int other_shared(void);

int main(void){
  printf("dup_a.c sees shared = %d\n", shared);
  printf("dup_b.c sees shared = %d\n", other_shared());
  return 0;
}
