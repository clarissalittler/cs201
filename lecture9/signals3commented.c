#include <stdlib.h>      // Standard library for general-purpose functions
#include <stdio.h>       // Standard I/O library for input/output functions
#include <stdbool.h>     // Boolean type and values (true, false)
#include <unistd.h>      // POSIX API for various system calls (e.g., getpid, sleep)
#include <signal.h>      // Signal handling functions and definitions

int main(){
    // Declare two signal sets:
    // 'blocks' will contain the signals we want to block
    // 'prevs' will store the previous signal mask before blocking
    sigset_t blocks, prevs;
    
    // Initialize the 'blocks' signal set to be empty
    if (sigemptyset(&blocks) == -1) {
        perror("sigemptyset failed");
        exit(EXIT_FAILURE);
    }
    
    // Add the SIGINT signal to the 'blocks' set
    // SIGINT is typically sent when the user presses Ctrl+C
    if (sigaddset(&blocks, SIGINT) == -1) {
        perror("sigaddset failed");
        exit(EXIT_FAILURE);
    }
    
    // Block the signals specified in 'blocks' (SIGINT in this case)
    // The current signal mask is saved into 'prevs' so it can be restored later if needed
    if (sigprocmask(SIG_BLOCK, &blocks, &prevs) == -1) {
        perror("sigprocmask failed");
        exit(EXIT_FAILURE);
    }
    
    // Enter an infinite loop
    while(true){
        // Print a message with the current process ID
        // This helps identify the running process
        printf("I bet you can't KILL me, mr. %u\n", getpid());
        
        // Pause execution for 1 second
        // This reduces CPU usage by not running the loop too rapidly
        sleep(1);
    }
    
    // This line is theoretically unreachable because of the infinite loop
    // However, it's good practice to include a return statement in main
    return 0;
}
