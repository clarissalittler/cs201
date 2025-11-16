#include <stdio.h>  // Includes standard input/output library for functions like printf and scanf
#include <stdlib.h> // Includes standard library for functions like exit and malloc
#include <unistd.h> // Includes POSIX operating system API functions for system calls like fork, wait, and getpid
#include <sys/types.h> // Includes definitions of data types used by the system, like pid_t
#include <sys/wait.h> // Includes definitions for functions related to process management like wait

int main(){

  pid_t pid = fork(); // Creates a child process. fork() returns the process ID of the child process in the parent process, and 0 in the child process.
  int returned; // Variable to store the exit status of the child process

  if(pid == 0){
    // in the child process
    int blah; // Variable to store the integer input from the user
    int results; // Variable to store the result of the scanf function
    printf("Say somethin', will ya: "); // Prompts the user to input an integer
    results = scanf("%d",&blah); // Reads an integer from the standard input stream and stores it in the variable 'blah'. Returns the number of items successfully read.
    if(results < 1){ // Checks if the scanf function read at least one item successfully.
      return 1; // If scanf did not read any items successfully, it means the user did not enter a valid integer. The child process exits with a return code of 1 to indicate an error.
    }
    else {
      return 0; // If scanf read at least one item successfully, the child process exits with a return code of 0 to indicate successful execution.
    }
  }
  else {
    // in the parent process
    wait(&returned); // Waits for the child process to terminate. The exit status of the child process is stored in the 'returned' variable.
  }
  printf("This was returned: %d\n",WEXITSTATUS(returned) ); // Prints the exit status of the child process. WEXITSTATUS extracts the exit status from the 'returned' variable.
  
  return 0; // The parent process exits with a return code of 0 to indicate successful execution.
}
