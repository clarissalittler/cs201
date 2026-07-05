#include <stdio.h>

// triggers a fault: SIGFPE, "floating point exception"
// (the name is historical -- this includes integer division by zero)
// the CPU itself raises an exception when it tries to divide by 0
// the kernel catches it, decides "this thread should die",
// and delivers the signal to the process
//
// faults are one of the four classes of exceptions:
//   interrupts (asynchronous, from devices)
//   faults     (synchronous, recoverable in principle, like page faults)
//   traps      (synchronous, intentional, like syscalls)
//   aborts     (synchronous, unrecoverable)
//
// run this and watch the shell print "Floating point exception"

int main(){
  int x = 10;
  int y = 0;
  // gcc may constant-fold a literal 10/0
  // so we keep y in a variable to make sure the divide actually happens at runtime
  printf("about to divide\n");
  int z = x / y;
  printf("never gets here: %d\n", z);
  return 0;
}
