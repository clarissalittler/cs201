#include <stdio.h>  // Includes standard input/output library for functions like printf
#include <stdlib.h> // Includes standard library for functions like exit
#include <unistd.h> // Includes POSIX operating system API for functions like fork
#include <sys/types.h> // Includes system types definitions, necessary for fork

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  // The fork() function creates a new process, which is a copy of the current process
  // It returns a value that helps distinguish between the parent and child processes
  // - In the parent process, fork() returns the process ID of the child
  // - In the child process, fork() returns 0
  // - If an error occurs, fork() returns -1
  int child_pid = fork();

  // If fork() was successful, it will print the message once for the parent and once for the child
  printf("This message should be printed by parent and child\n");
  
  // This line exits the main function with a return code of 0, indicating successful program execution
  return 0; 
}
