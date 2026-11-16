// Where did everything end up in memory?
//
// This prints a few addresses, then the process's own memory map from
// /proc/self/maps. Each line of the map is one region: its address range,
// its permissions, and the file it came from (if any).
//
// try:
//   ./whereami
//   ./whereami          again: the addresses change (ASLR), the layout doesn't
//   ./peek.sh whereami  compare the LOAD segments with the r-xp / rw-p lines
//
// Things to find in the map:
//   - main is in an r-xp region backed by our program file
//   - the global is in a rw-p region backed by our program file
//   - printf is in an r-xp region backed by libc.so.6
//   - the malloc'd block is in [heap]; the local is in [stack]
//   - ld-linux-x86-64.so.2 is mapped too: it's the loader that did all this

#include <stdio.h>
#include <stdlib.h>

int global = 1;

int main(void){
  int local = 2;
  int* heap = malloc(sizeof(int));
  if(heap == NULL){
    perror("malloc");
    return 1;
  }

  printf("main     (our code)       %p\n", (void*) main);
  printf("global   (our data)       %p\n", (void*) &global);
  printf("printf   (libc's code)    %p\n", (void*) printf);
  printf("heap     (malloc)         %p\n", (void*) heap);
  printf("local    (the stack)      %p\n", (void*) &local);
  printf("\n");
  fflush(stdout);

  FILE* maps = fopen("/proc/self/maps", "r");
  if(maps == NULL){
    perror("/proc/self/maps");
    free(heap);
    return 1;
  }
  char line[512];
  while(fgets(line, sizeof line, maps) != NULL){
    fputs(line, stdout);
  }
  fclose(maps);

  free(heap);
  return 0;
}
