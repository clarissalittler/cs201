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
   * Variable to store status information
   */
  int returned;

  
  if(pid < 0){
    perror("Fork failed");
    return 1;
  }
  else if(pid == 0){
    /* 
     * Child process - handles user input and returns
     * different exit codes based on input success
     */
    int blah;
    printf("Say somethin', will ya: ");
    returned = scanf("%d",&blah);
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
     * and captures its exit status
     */
    wait(&returned);
  }

  /* 
   * TEACHING POINT: This demonstrates a buggy way of extracting
   * the exit status from the wait() return value. It shows what
   * NOT to do when handling exit statuses.
   * 
   * The correct way is to use WEXITSTATUS(returned) as in pid3.c
   * 
   * This line attempts to extract the exit code with bit manipulation:
   * - Shift right by 8 bits (since exit status is in bits 8-15)
   * - AND with 0xFF to mask out other bits
   * 
   * But there are two bugs:
   * 1. "&&" is used instead of "&" (logical AND vs bitwise AND)
   * 2. The bit shift is wrong - it shifts all the bits that matter out
   *    (& evaluates to either 0 or 1, then && with 0xFF is always 0 or 1)
   * 
   * This demonstrates common errors when working with exit status values.
   */
  returned = (returned >> 8) && 0xFF;
  if(returned  == 1){
    printf("They massacred my boy!\n");
  }
  else{
    printf("Everything's great, isn't it?\n");
  }
  return 0;
}