#include <stdio.h>      // Standard Input/Output functions
#include <stdlib.h>     // Standard library functions like exit()
#include <unistd.h>     // POSIX operating system API, including fork()
#include <sys/types.h>  // Data types used in system calls
#include <sys/wait.h>   // Declarations for waiting (wait, waitpid)

int main(){
    // Declare a variable to hold process ID returned by fork()
    pid_t pid = fork();
    
    // Variable to store the status information returned by wait()
    int returned;
    
    // Check if fork() failed
    if(pid < 0){
        // Print an error message to stderr if fork fails
        perror("Fork failed");
        return 1; // Exit the program with a non-zero status to indicate failure
    }
    // Child process block
    else if(pid == 0){
        int blah; // Variable to store user input
        // Prompt the user for input
        printf("Say somethin', will ya: ");
        
        // Read an integer from standard input and store it in 'blah'
        // 'returned' will hold the number of items successfully read
        returned = scanf("%d", &blah);
        
        // Check if scanf successfully read at least one item
        if(returned < 1){
            // If not, exit with status 1 indicating an error
            return 1;
        }
        else{
            // If successful, exit with status 0 indicating success
            return 0;
        }
    }
    // Parent process block
    else{
        // Parent waits for the child process to terminate
        // The exit status of the child is stored in 'returned'
        wait(&returned);
    }

    // After the child has terminated, check its exit status
    // WEXITSTATUS extracts the lower 8 bits of the exit status
    if(WEXITSTATUS(returned) == 1){
        // If the child exited with status 1, print an error message
        printf("They massacred my boy!\n");
    }
    else{
        // If the child exited with status 0, print a success message
        printf("Everything's great, isn't it?\n");
    }
    
    // Exit the parent process with status 0 indicating successful execution
    return 0;
}
