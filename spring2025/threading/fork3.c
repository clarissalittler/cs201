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
    // anti-zombie ward
    wait(NULL);
    printf("I'm the parent!\n");
  }
  else{
    printf("I'm the child!\n");
  }
  // this is the first line the child sees
  printf("This message should be printed by parent and child\n");
  
  return 0;
}
