#include <stdio.h> // Includes standard input/output library for functions like printf
#include <stdlib.h> // Includes standard library for functions like exit
#include <unistd.h> // Includes POSIX operating system API functions like fork and wait
#include <sys/types.h> // Includes data types used by system calls like pid_t
#include <sys/wait.h> // Includes functions for waiting for child processes like wait

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  pid_t pid = fork(); // This line calls the fork() function. fork() creates a new process (the child) that is a copy of the current process (the parent). 
                      // It returns the process ID (PID) of the child process in the parent process. In the child process, it returns 0.

  int result; // This variable will store the exit status of the child process

  if(pid != 0){ // This checks if the value returned by fork() is not 0. This means we are in the parent process.
    printf("I'm the parent!\n"); // Prints a message indicating the parent process.
    wait(&result); // This calls the wait() function, which suspends the parent process until one of its child processes terminates. 
                   // The exit status of the child process is stored in the result variable.
    printf("My child returned: %d\n",result); // Prints the exit status of the child process.
  }
  else{ // This block of code is executed only if the value returned by fork() is 0, indicating that we are in the child process.
    printf("I'm the child!\n"); // Prints a message indicating the child process.
  }
  // this is the first line the child sees 
  printf("This message should be printed by parent and child\n"); // This line is executed by both the parent and child processes as it comes after the fork() call.

  return 0; // Returns 0 to indicate successful program execution.
}
