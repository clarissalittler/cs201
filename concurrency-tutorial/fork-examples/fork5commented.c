#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

  /* 
   * Create a child process and get its PID
   */
  pid_t pid = fork();
  
  /* 
   * Variable to store exit status information
   * In parent: filled by wait() with child's exit status
   * In child: used to store scanf result
   */
  int returned;

  if(pid == 0){
    // in the child process
    
    /* 
     * Child-only variables
     * - 'blah' stores user input
     * - 'results' stores scanf return value (number of items read)
     */
    int blah;
    int results;
    
    /* 
     * Prompt user for input
     * The child process handles user interaction
     */
    printf("Say somethin', will ya: ");
    results = scanf("%d",&blah);
    
    /* 
     * IMPORTANT TEACHING POINT:
     * Checking scanf's return value demonstrates proper error handling.
     * If scanf fails (returns < 1), we return exit code 1 (error)
     * Otherwise, we return exit code 0 (success)
     * 
     * This shows how a child process can communicate status back to the parent
     * through its exit code.
     */
    if(results < 1){
      return 1;  // Error in input
    }
    else {
      return 0;  // Successful input
    }
  }
  else {
    // in the parent process
    
    /* 
     * Parent waits for child to complete and captures its exit status
     * This demonstrates synchronization between processes
     */
    wait(&returned);
  }
  
  /* 
   * TEACHING POINT:
   * This line demonstrates proper use of WEXITSTATUS macro to extract
   * the actual exit code from the status word.
   * 
   * This is executed by the parent only (the child has already returned).
   * It shows how to properly interpret the exit status value.
   */
  printf("This was returned: %d\n",WEXITSTATUS(returned) );
  
  return 0;
}