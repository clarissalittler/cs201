#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(){

  int num1 = 10; // here there's only one num1
  
  pid_t pid = fork(); // create a new process that's a CLONE of the original
  // will put the new process ID in the variable pid
  // but ONLY the parent will have a PID here
  // the child will have a variable that holds 0

  // here each process gets its own num1
  // fork clones *everything*
  // fork clones the memory space
  // state of registers
  // stack
  // everything that makes a process a process
  
  if(pid != 0){
    num1++;
    printf("I'm the parent. Here's my number  %d\n",num1);
  }
  else{
    num1--;
    printf("I'm the child. Here's my number %d\n",num1);
  }
  printf("And we both say hi\n");
  // fork is the earlier concurrency primitive
  
  
  return 0;
}
