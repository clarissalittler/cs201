#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  
  printf("We haven't forked yet\n"); // at this point there's only one process
  fork(); // after this line executes, there are now two processes
  printf("We've now forked\n"); // so here two separate processes run this line
  
  return 0;
}
