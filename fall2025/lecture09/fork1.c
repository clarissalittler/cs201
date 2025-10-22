#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(){

  printf("How's it going?\n");
  
  fork(); // create a new process that's a CLONE of the original

  printf("Hi there\n");
  
  // pid_t pid = fork();
  
  return 0;
}
