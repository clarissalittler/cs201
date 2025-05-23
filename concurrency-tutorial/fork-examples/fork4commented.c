#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  /* 
   * Create a child process and get its PID
   */
  pid_t pid = fork(); // the parent forks, gets the pid in this variable

  /* 
   * Variable to store the return status from wait()
   * 
   * NOTE: This variable is created in BOTH parent and child processes
   * But it's only meaningful in the parent process
   */
  int result;
  
  if(pid != 0){
    printf("I'm the parent!\n");
    
    /* 
     * The parent waits for the child to terminate
     * and stores the exit status in 'result'
     * 
     * IMPORTANT TEACHING POINT:
     * There's a subtle bug here - the wait() call puts the exit status
     * into 'result', but the program doesn't handle the exit status correctly.
     * It prints the raw status value, not the actual exit code.
     * 
     * The exit status is encoded in a way that requires macros like
     * WEXITSTATUS() to extract the actual return value. This demonstrates
     * the complexity of working with process exit values.
     */
    wait(&result);
    printf("My child returned: %d\n",result);
  }
  else{
    printf("I'm the child!\n");
  }
  // this is the first line the child sees
  printf("This message should be printed by parent and child\n");
  
  return 0;
}