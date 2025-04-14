#include <stdlib.h>   // Standard utility library for functions like exit()
#include <stdio.h>    // Standard I/O library for input/output functions like printf()
#include <stdbool.h>  // Boolean type support
#include <unistd.h>   // POSIX operating system API, includes alarm() and pause()
#include <signal.h>   // Signal handling library for managing signals like SIGALRM

// Function prototype for the signal handler
void alarm_handler(int sig){
    // This function is called when the SIGALRM signal is received
    // 'sig' parameter holds the signal number, which is SIGALRM in this case
    printf("The bells have been rung!\n");
}

int main(){
    // Inform the user about the program's behavior
    printf("This program will end once the bells have been rung\n");
    
    // Register the alarm_handler function as the handler for SIGALRM signals
    // This means that when SIGALRM is received, alarm_handler will be invoked
    signal(SIGALRM, alarm_handler);
    
    // Schedule an alarm signal to be sent to the process after 5 seconds
    alarm(5);
    
    // Inform the user that the program is now waiting/sleeping
    printf("We slumber\n");
    
    // Suspend the program until any signal is received
    // In this case, the program will pause until the SIGALRM is received after 5 seconds
    pause();
    
    // After the signal is handled and the program resumes, this line will be executed
    printf("We have awoken!\n");
    
    // Return 0 to indicate successful execution
    return 0;
}
