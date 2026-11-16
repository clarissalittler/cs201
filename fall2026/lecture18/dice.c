// Roll five dice with rand() from the C library.
//
// rand() isn't in our program. It's in libc.so, and the loader connects our
// call to it at run time. That means we can get in the middle: LD_PRELOAD
// loads a library of our choice *first*, and its rand() wins.
//
// try:
//   ./dice
//   ./dice                                         different every time
//   LD_PRELOAD=./fakerand.so ./dice                 hmm
//   LD_DEBUG=bindings ./dice 2>&1 | grep -w rand    who got rand?
//   LD_DEBUG=bindings LD_PRELOAD=./fakerand.so ./dice 2>&1 | grep -w rand
//
//   objdump -d --disassemble=main dice              call ... <rand@plt>
//
// The @plt is a little stub the linker added. The call goes to the stub, and
// the stub jumps to wherever the loader decided rand really is.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void){
  srand((unsigned) time(NULL));
  printf("You rolled:");
  for(int i = 0; i < 5; i++){
    printf(" %d", rand() % 6 + 1);
  }
  printf("\n");
  return 0;
}
