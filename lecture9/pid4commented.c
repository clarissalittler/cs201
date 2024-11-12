#include <stdio.h>  // Includes standard input/output library for functions like printf and scanf
#include <stdlib.h> // Includes standard library for functions like perror and exit
#include <unistd.h> // Includes POSIX operating system API functions, including fork and wait
#include <sys/types.h> // Includes system types, such as pid_t (process ID)
#include <sys/wait.h> // Includes functions for managing processes, including wait

int main(){

  pid_t pid = fork(); // Calls fork() to create a child process.
                     //  - pid is set to the child process's ID if successful, 
                     //  - 0 if it's the child process itself, 
                     //  - and -1 if there's an error.

  int returned;  // Declares an integer variable to store the return value from the child process.

  // Checks if fork failed. If so, prints an error message and exits the program.
  if(pid < 0){
    perror("Fork failed");  // Prints an error message to stderr (standard error output)
    return 1;  // Exits the program with an error code
  }

  // Executes code in the child process (pid == 0).
  else if(pid == 0){
    int blah;  // Declares an integer variable to store user input.
    printf("Say somethin', will ya: ");  // Prompts the user to enter a number.
    returned = scanf("%d",&blah);  // Reads an integer from standard input and stores it in 'blah'.
                                   // - returned will be the number of items successfully read (1 in this case) or 0 if there's an error.

    // Checks if scanf failed. If so, exits the child process with an error code.
    if(returned < 1){
      return 1;  // Exits the child process with an error code.
    }
    // If scanf succeeded, exits the child process with a success code.
    else{
      return 0; // Exits the child process with a success code.
    }
  }
  // Executes code in the parent process (pid > 0).
  else{
    wait(&returned); // Suspends the parent process until the child process terminates, storing the child's exit status in 'returned'.
  }

  // Extracts the exit code of the child process from 'returned'.
  // - shifts the bits in 'returned' 8 places to the right, 
  // - then performs a bitwise AND operation with 0xFF to isolate the last byte (representing the exit code).
  returned = (returned >> 8) && 0xFF; 

  // Checks if the child process exited with an error code.
  if(returned  == 1){
    printf("They massacred my boy!\n");  // Prints a message indicating the child process failed.
  }
  // If the child process exited with a success code.
  else{
    printf("Everything's great, isn't it?\n");  // Prints a message indicating the child process succeeded.
  }

  return 0;  // Exits the parent process with a success code.
}
