#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

  printf("Hello! My id is %d and my parent's id is %d\n",getpid(),getppid());
  
  return 0;
}
