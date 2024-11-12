#include <stdio.h>  // Include standard input/output library for functions like printf
#include <stdlib.h> // Include standard library for functions like perror
#include <unistd.h> // Include POSIX operating system API for functions like fork and wait
#include <sys/types.h> // Include system types definitions, needed for pid_t
#include <sys/wait.h>  // Include wait functions for managing child processes

int main() {

  pid_t pid = fork(); // Call fork() to create a new process.
                     // fork() returns:
                     // - a child process ID (pid) in the child process
                     // - 0 in the parent process
                     // - -1 on error

  if (pid < 0) { // Check if fork() failed
    perror("Fork failed"); // Print the error message to stderr
    return 1; // Return 1 to indicate an error
  } 
  else if (pid == 0) { // This block executes only in the child process
    printf("This is the child process.\n"); // Print a message to indicate the child process
  }
  else { // This block executes only in the parent process
    wait(0); // Wait for the child process to complete. 
             // 0 as the argument indicates waiting for any child process.
             // The wait() function will block the parent process until the child process terminates.
    printf("This is the parent process.\n"); // Print a message to indicate the parent process
  }
  printf("This message is printed by both the parent and the child\n"); // This message will be printed by both processes because both processes continue execution after their respective if-else blocks.

  return 0; // Indicate successful program execution
}
