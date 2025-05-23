#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  /* 
   * This is the simplest example of fork.
   * Here we call fork() which creates a child process.
   * The child process is an exact copy of the parent
   * process, including memory and execution position.
   * 
   * The key teaching point here is that after fork():
   * - We now have TWO processes running
   * - Both will continue from the line after fork()
   * - Both processes will execute the same code
   * - No differentiation between parent and child is made
   */
  fork();

  /* 
   * This line will be executed by BOTH the parent and child processes.
   * When you run this program, you'll see this message printed twice.
   * 
   * The demonstration shows that forking creates a complete duplicate
   * of the running process. Both continue execution independently
   * from the same point.
   */
  printf("This message should be printed by parent and child\n");
  
  return 0;
}