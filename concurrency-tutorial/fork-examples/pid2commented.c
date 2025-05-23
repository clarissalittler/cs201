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
   * Complete error handling for fork()
   * Demonstrating the three possible return values from fork:
   * -1: fork failed
   * 0: we're in the child process
   * positive: we're in the parent (value is child's PID)
   */
  if(pid < 0){
    perror("Fork failed");
    return 1;
  }
  else if(pid == 0){
    /* 
     * Child process code path
     * Simply prints a message to identify itself
     */
    printf("This is the child process.\n");
  }  
  else{
    /* 
     * Parent process code path
     * 
     * TEACHING POINT: Proper process synchronization
     * The parent uses wait(0) to wait for ANY child to terminate
     * (passing 0/NULL means we don't care about the exit status)
     * 
     * This ensures the child finishes before the parent continues,
     * which creates predictable output ordering.
     */
    wait(0);
    printf("This is the parent process.\n");
  }
  
  /* 
   * TEACHING POINT: The parent will only reach this after the child
   * has terminated (due to the wait call), whereas the child
   * executes this immediately.
   * 
   * This demonstrates how wait() affects execution flow and 
   * synchronization between processes.
   */
  printf("This message is printed by both the parent and the child\n");
  return 0;
}