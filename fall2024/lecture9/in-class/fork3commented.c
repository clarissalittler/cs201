#include <stdio.h>  // Standard input/output library (for printf)
#include <stdlib.h> // Standard library (for exit)
#include <unistd.h> // POSIX operating system API (for fork, wait)
#include <sys/types.h> // System types (for pid_t)
#include <sys/wait.h> // Wait functions (for wait)

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  pid_t pid = fork(); // the parent forks, gets the pid in this variable
                    // fork() creates a new process, called a child process, that's a copy of the parent process
                    // It returns the process ID of the child process to the parent, and 0 to the child

  if(pid != 0){
    // pid != 0 is true for the parent process, since fork() returns the child's PID to the parent
    printf("I'm the parent!\n");
  }
  else{
    // pid == 0 is true for the child process, since fork() returns 0 to the child
    printf("I'm the child!\n");
  }
  // anti-zombie ward
  wait(NULL); // Wait for the child process to terminate. 
              // This is important to avoid "zombie" processes, which are processes that have terminated
              // but still exist in the process table. This can consume resources.
              // NULL tells wait() to wait for any child process.
  // this is the first line the child sees
  printf("This message should be printed by parent and child\n"); 
  // Both the parent and child processes will execute this line, since fork() creates a copy of the parent process.
  
  return 0;
}
