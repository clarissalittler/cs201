#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  pid_t pid = fork(); // the parent forks, gets the pid in this variable
  
  if(pid != 0){
    int result;
    printf("I'm the parent!\n");
    wait(&result);
    // wexitstatus(r) ==> (r >> 8) & 255
    printf("My child returned: %d\n",WEXITSTATUS(result));
  }
  else{
    printf("I'm the child!\n");
    return 2;
  }

  return 0;
}
