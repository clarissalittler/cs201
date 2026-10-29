#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

int main(){

  long pageSize = sysconf(_SC_PAGESIZE);
  printf("How big is a page? %ld \n",pageSize);
  
  return 0;
}
