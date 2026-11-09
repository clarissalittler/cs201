// Putting it together: asking the kernel about ourselves through a "file"
//
// /proc/self/status is a text file the kernel writes on the fly,
// describing whatever process opened it. Since it's "just a file,"
// we can read it with fopen and fgets like anything else.
//
// try:
//   ./procSelf
//   cat /proc/self/status       (that's cat describing *itself*)
//   ls /proc/self/              (lots more in there)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void){
  // grab some memory so VmRSS has something to show
  char* big = malloc(50 * 1024 * 1024);
  memset(big, 1, 50 * 1024 * 1024);   // touch it so it's really in RAM

  FILE* f = fopen("/proc/self/status", "r");
  if(f == NULL){
    perror("/proc/self/status");
    return 1;
  }

  const char* wanted[] = { "Name:", "Pid:", "PPid:", "Threads:", "VmRSS:" };

  char line[256];
  while(fgets(line, sizeof(line), f) != NULL){
    for(int i = 0; i < 5; i++){
      // does this line start with one of the fields we want?
      if(strncmp(line, wanted[i], strlen(wanted[i])) == 0){
        printf("%s", line);
      }
    }
  }

  fclose(f);
  free(big);
  return 0;
}
