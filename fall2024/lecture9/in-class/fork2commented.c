#include <stdio.h>  // Include the standard input/output library for functions like printf
#include <stdlib.h> // Include the standard library for general utilities, such as memory allocation
#include <unistd.h> // Include the POSIX operating system API for system calls, including fork
#include <sys/types.h> // Include the system types header file, which defines data types used in system calls, including pid_t

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  pid_t pid = fork(); // Calls the fork() system call. It creates a new process, a copy of the current process.
                      // The fork() function returns a value:
                      // - In the parent process, it returns the process ID (PID) of the newly created child process.
                      // - In the child process, it returns 0. 
                      // - If fork() fails, it returns -1.

  if(pid != 0){  //  Checks if the return value of fork() is not 0, which means this is the parent process.
    printf("I'm the parent!\n");
  }
  else{         // If the return value is 0, then this is the child process.
    printf("I'm the child!\n");
  }

  // This is the first line the child sees. Both parent and child execute this line because fork() creates a copy.
  printf("This message should be printed by parent and child\n");
  
  return 0; // The program exits successfully
}
