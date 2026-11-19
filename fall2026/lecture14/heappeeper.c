#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  printf("Initial break: %p\n", sbrk(0)); // this lets us see the initial heap boundary

  void* small = malloc(1000);
  
  printf("After small malloc: %p\n", sbrk(0));
  printf("Small allocation: %p\n", small);
  
  void* large = malloc(10 * 1024 * 1024); // 2560 pages
  
  printf("After biiig malloc: %p\n", sbrk(0));
  printf("New : %p\n", large);
  
  puts("Now go look at the memory map with cat /proc/$(pgrep -n heappeeper)/maps");
  getchar();
  
  free(small);
  free(large);
  
  return 0;
}
