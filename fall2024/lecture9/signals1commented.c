#include <stdio.h>      // Standard input/output library for functions like printf
#include <signal.h>     // Library for handling signals like SIGINT
#include <unistd.h>     // Provides access to the POSIX operating system API, including sleep
#include <stdbool.h>    // Defines boolean type and values 'true' and 'false'
#include <stdlib.h>     // Standard library for functions like exit

// Declare a global variable 'counter' to keep track of the number of SIGINT signals received
// 'volatile sig_atomic_t' is the usual type for signal-shared flags/counters.
volatile sig_atomic_t counter = 0;

// Define the signal handler function for SIGINT
void sigint_handler(int sig){
    (void)sig; // The example only cares that SIGINT happened.

    // IMPORTANT SIGNAL-HANDLER RULE:
    // Keep the handler tiny. We only update a signal-safe counter here.
    if(counter < 3){
        counter++;
    }
}

int main(){
    sig_atomic_t reported = 0;
    // Print an initial message indicating that a SIGINT handler is set up
    printf("We have a ctrl-c handler here!\n");
    
    // Set up the signal handler for SIGINT (Ctrl-C)
    // When SIGINT is received, 'sigint_handler' will be called
    signal(SIGINT, sigint_handler);
    
    // Enter an infinite loop that continuously prints "Boop boop" every second
    while(true){
        // Print one status line for each newly received Ctrl-C.
        while(reported < counter){
            reported++;
            printf("Caught a sigint: Press ctrl-c %d more times to exit\n", 3 - reported);
        }

        if(counter >= 3){
            printf("I've been banished!\n");
            break;
        }

        printf("Boop boop\n"); // Print a message to the console
        sleep(1);              // Pause execution for 1 second
    }
    
    // Although the loop is infinite, return 0 to indicate successful termination
    // (This line will never be reached in this program)
    return 0;
}

/* Example session:
We have a ctrl-c handler here!
Boop boop
Boop boop
^CCaught a sigint: Press ctrl-c 2 more times to exit
Boop boop
Boop boop
^CCaught a sigint: Press ctrl-c 1 more times to exit
Boop boop
Boop boop
^CCaught a sigint: Press ctrl-c 0 more times to exit
I've been banished!
*/
