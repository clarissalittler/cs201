#include <stdio.h>  // Includes standard input/output library for functions like printf and scanf
#include <stdlib.h> // Includes standard library for general functions like malloc, exit, etc.
#include <unistd.h> // Includes POSIX operating system API for functions like fork, wait, etc.
#include <sys/types.h> // Includes system types for data types used in system calls
#include <sys/wait.h> // Includes wait functions for managing child processes

int main(){

  pid_t pid = fork(); // Creates a new process. 'pid' stores the process ID of the child process
                     // If fork() fails, it returns -1. If it succeeds, it returns 0 in the child process and the child's process ID in the parent process.

  int returned; // Declares an integer variable to store the return value from the child process.

  
  if(pid < 0){ // If fork() failed
    perror("Fork failed"); // Prints an error message to stderr (standard error)
    return 1; // Exits the program with an error code
  }
  else if(pid == 0){ // If this is the child process
    int blah; // Declares an integer variable to store user input
    printf("Say somethin', will ya: "); // Prompts the user for input
    returned = scanf("%d",&blah); // Reads an integer from the user input and stores it in the 'blah' variable.
                               // 'returned' stores the number of items successfully read.
    if(returned < 1){ // If scanf failed to read an integer
      return 1; // Exits the child process with an error code
    }
    else{
      return 0; // Exits the child process successfully
    }
  }
  else{ // If this is the parent process
    wait(&returned); // Waits for the child process to terminate. 
                    // The exit status of the child process is stored in the 'returned' variable.
  }

  if(WEXITSTATUS(returned) == 1){ // Checks the child process's exit status. WEXITSTATUS() extracts the child process's exit code.
    printf("They massacred my boy!\n"); // Prints a message if the child process exited with an error.
  }
  else{
    printf("Everything's great, isn't it?\n"); // Prints a message if the child process exited successfully.
  }
  return 0; // Exits the parent process successfully
}
