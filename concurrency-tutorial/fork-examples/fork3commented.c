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
   * As in the previous example, we capture the fork return value
   * to differentiate between parent and child processes.
   */
  pid_t pid = fork(); // the parent forks, gets the pid in this variable

  /* 
   * Using the pid value to determine if we're in parent or child process.
   */
  if(pid != 0){
    printf("I'm the parent!\n");
  }
  else{
    printf("I'm the child!\n");
  }
  
  /* 
   * This is the zombie process prevention mechanism.
   * 
   * IMPORTANT TEACHING POINT: This wait() actually introduces a bug!
   * The wait() is called by BOTH parent and child, but:
   * - In the parent: It waits for the child to finish (as intended)
   * - In the child: The child has no children to wait for, so this
   *   wait() will either return immediately (if there are no children)
   *   or block indefinitely (if the parent is expecting a status)
   * 
   * This demonstrates a common error when using wait() - forgetting
   * that both processes execute the same code unless specifically
   * directed otherwise.
   */
  wait(NULL);
  
  // this is the first line the child sees
  printf("This message should be printed by parent and child\n");
  
  return 0;
}