#include <stdio.h>

// the other classic fault: SIGSEGV, "segmentation fault"
// dereferencing a NULL pointer triggers a page fault on an unmapped page
// the kernel checks the page table, sees the page isn't mapped,
// can't recover, and signals the process

int main(){
  int* p = NULL;
  printf("about to dereference NULL\n");
  *p = 42;
  printf("never gets here\n");
  return 0;
}
