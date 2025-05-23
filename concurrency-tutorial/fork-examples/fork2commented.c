#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  /* 
   * Here we capture the return value from fork().
   * This is how we distinguish between parent and child:
   * - In the parent process: pid contains the child's process ID (a positive number)
   * - In the child process: pid contains 0
   * - If fork fails: pid contains -1
   *
   * This demonstrates a critical concept in process creation:
   * fork() returns DIFFERENT values to the parent and child processes.
   */
  pid_t pid = fork(); // the parent forks, gets the pid in this variable

  /* 
   * This conditional checks if we're in the parent process (pid != 0).
   * The parent receives the child's PID; the child receives 0.
   * 
   * The teaching point is that after fork, we have two identical processes
   * running the same code, but they can execute different paths based
   * on the return value from fork().
   */
  if(pid != 0){
    printf("I'm the parent!\n");
  }
  else{
    printf("I'm the child!\n");
  }
  // this is the first line the child sees
  printf("This message should be printed by parent and child\n");
  
  return 0;
}