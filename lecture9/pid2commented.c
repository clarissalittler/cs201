#include <stdio.h>      // Standard input/output library for printf and perror functions
#include <stdlib.h>     // Standard library for general-purpose functions
#include <unistd.h>     // UNIX standard library for fork and other POSIX functions
#include <sys/types.h>  // Defines data types used in system calls
#include <sys/wait.h>   // Wait functions for process control

int main(){
    // Fork a new process
    pid_t pid = fork();

    // Check if fork() failed
    if(pid < 0){
        // perror prints a descriptive error message to stderr
        perror("Fork failed");
        return 1; // Exit the program with a non-zero status indicating failure
    }
    // Child process block
    else if(pid == 0){
        // This code is executed only by the child process
        printf("This is the child process.\n");
    }  
    // Parent process block
    else{
        // Parent process waits for the child process to complete
        // wait(0) is equivalent to wait(NULL), which waits for any child process
        wait(0);
        // After child has finished, parent prints its message
        printf("This is the parent process.\n");
    }
    
    // This line is executed by both parent and child processes
    printf("This message is printed by both the parent and the child\n");
    
    return 0; // Exit the program successfully
}
This is the child process.
This message is printed by both the parent and the child
This is the parent process.
This message is printed by both the parent and the child
