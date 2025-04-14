#include <stdio.h>      // Standard Input/Output library for functions like printf and scanf
#include <stdlib.h>     // Standard Library for functions like exit
#include <unistd.h>     // POSIX API for functions like fork
#include <sys/types.h>  // Data types used in system calls (e.g., pid_t)
#include <sys/wait.h>   // Definitions for waiting functions like wait

int main(){
    // Fork the current process to create a child process
    pid_t pid = fork();  
    // 'pid' will hold the process ID returned by fork()
    // If pid < 0, fork failed
    // If pid == 0, this is the child process
    // If pid > 0, this is the parent process

    int returned; // Variable to store the status information from child

    // Check if fork() failed
    if(pid < 0){
        perror("Fork failed"); // Print an error message to stderr
        return 1;             // Exit the program with a status code of 1 indicating failure
    }
    // Child process block
    else if(pid == 0){
        int blah; // Variable to store the user input integer

        // Prompt the user for input
        printf("Say somethin', will ya: ");

        // Read an integer from the standard input and store it in 'blah'
        // 'returned' will hold the number of items successfully read
        returned = scanf("%d", &blah);

        // Check if scanf successfully read an integer
        if(returned < 1){
            // If not, exit the child process with status code 1
            return 1;
        }
        else{
            // If successful, exit the child process with status code 0
            return 0;
        }
    }
    // Parent process block
    else{
        // Wait for the child process to complete
        // The exit status of the child is stored in 'returned'
        wait(&returned);
    }

    // Shift the exit status to get the actual return code from the child
    // The wait() function encodes the exit status in the higher-order bits
    returned = (returned >> 8) && 0xFF;
    // Explanation:
    // - 'returned >> 8' shifts the status to get the exit code
    // - '&& 0xFF' ensures that only the lower 8 bits are considered

    // Check the exit status of the child process
    if(returned == 1){
        // If the child exited with status 1, print an error message
        printf("They massacred my boy!\n");
    }
    else{
        // If the child exited with status 0, print a success message
        printf("Everything's great, isn't it?\n");
    }

    return 0; // Exit the parent process with status code 0 indicating success
}
