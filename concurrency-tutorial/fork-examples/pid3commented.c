#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

  /* 
   * Create a child process and store its PID
   */
  pid_t pid = fork();
  
  /* 
   * Variable with different purposes in parent and child:
   * - In child: Stores result of scanf()
   * - In parent: Receives child's exit status via wait()
   * 
   * TEACHING POINT: After fork(), each process has its own copy
   * of all variables. Changes to variables in one process don't
   * affect the other process.
   */
  int returned;

  
  if(pid < 0){
    perror("Fork failed");
    return 1;
  }
  else if(pid == 0){
    /* 
     * Child process code path
     * 
     * TEACHING POINT: This demonstrates how a child process can
     * perform tasks and communicate results back to the parent
     * through its exit status.
     */
    int blah;
    printf("Say somethin', will ya: ");
    returned = scanf("%d",&blah);
    
    /* 
     * Return different exit codes based on input success
     * This is a key mechanism for process communication
     */
    if(returned < 1){
      return 1;  // Error exit code
    }
    else{
      return 0;  // Success exit code
    }
  }
  else{
    /* 
     * Parent process waits for child to complete
     * and captures its exit status in 'returned'
     */
    wait(&returned);
  }

  /* 
   * TEACHING POINT: Using WEXITSTATUS macro to extract actual
   * exit code from the status word. This shows proper status handling.
   * 
   * This demonstrates how to check the child's return value
   * and take different actions based on success/failure.
   * 
   * Note that only the parent executes this code - the child
   * has already terminated with a return statement.
   */
  if(WEXITSTATUS(returned) == 1){
    printf("They massacred my boy!\n");
  }
  else{
    printf("Everything's great, isn't it?\n");
  }
  return 0;
}