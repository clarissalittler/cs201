#include <stdio.h>  //  Includes standard input/output functions like printf and scanf
#include <stdlib.h> // Includes standard library functions like atoi (convert string to integer)
#include <unistd.h> // Includes POSIX operating system API functions like fork, pipe, read, and write
#include <string.h> // Includes string manipulation functions like strlen
#include <sys/types.h> // Includes system data types for interacting with the kernel
#include <sys/wait.h> // Includes wait functions for handling child processes

// We want a program that will fork
// a process, read a number from stdin
// via scanf and then if the scanf was
// successful, send that number to the
// parent

int main(){

  int pipeFileDesc[2]; // This array will hold the file descriptors for the pipe.
  // pipeFileDesc[0] will be the read end, pipeFileDesc[1] will be the write end.
  int returned; // This variable will store the exit status of the child process.
  int output; // This variable is not used.

  pipe(pipeFileDesc); // Creates an anonymous pipe. The pipe is a unidirectional communication channel
  // between two processes.
  // After this point, pipeFileDesc[0] should be the read end 
  // and pipeFileDesc[1] should be the write end.

  pid_t pid = fork(); // Forks a child process.
  // pid is the process ID of the child process.
  // If pid == 0, we are in the child process.
  // If pid > 0, we are in the parent process.
  // If pid == -1, there was an error forking.

  if( pid == 0){
    // in the child process
    int blah; // This variable will store the number read from stdin.
    int results; // This variable will store the result of scanf.
    printf("Say somethin', will ya: "); // Prompts the user for input.
    results = scanf("%d",&blah); // Reads an integer from stdin and stores it in the variable blah.
    // The return value of scanf indicates the number of items successfully read.
    if(results < 1){
      //scanf failed
      return 1; // Exits the child process with an error code (1).
    }
    else{
      //scanf succeded
      char buf[10]; // This array will store the integer as a string.
      sprintf(buf,"%d",blah); // Converts the integer blah to a string and stores it in the buffer buf.
      write(pipeFileDesc[1],buf,strlen(buf)+1); // Writes the string in buf to the write end of the pipe.
      // strlen(buf)+1 includes the null terminator '\0' at the end of the string.
      close(pipeFileDesc[1]); // Closes the write end of the pipe in the child process.
      close(pipeFileDesc[0]); // Closes the read end of the pipe in the child process.
      return 0; // Exits the child process successfully.
    }
  }
  else {
    // in the parent process
    wait(&returned); // Waits for the child process to terminate.
    // The exit status of the child process is stored in the variable returned.
    if(WEXITSTATUS(returned) == 1){ 
      // WEXITSTATUS(returned) extracts the exit status of the child process from the returned value.
      // If the child process exited with an error code (1), this branch will execute.
      printf("I guess there was nothing to read\n"); // Prints a message indicating that the child process failed to read input.
      close(pipeFileDesc[1]); // Closes the write end of the pipe in the parent process.
      close(pipeFileDesc[0]); // Closes the read end of the pipe in the parent process.
    }
    else{
      char outBuf[10]; // This array will store the string read from the pipe.
      read(pipeFileDesc[0],outBuf,10); // Reads up to 10 bytes from the read end of the pipe and stores them in the buffer outBuf.
      // The actual number of bytes read is determined by the size of the string sent by the child process.

      printf("I got the number: %d\n",atoi(outBuf)); // Prints the number read from the pipe.
      // atoi(outBuf) converts the string in outBuf to an integer.
    }
  }

  return 0; // Exits the parent process successfully.
}
