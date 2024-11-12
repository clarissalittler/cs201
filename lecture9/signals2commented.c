#include <stdlib.h> //  Includes standard library functions like malloc(), free(), etc.
#include <stdio.h>  //  Includes standard input/output functions like printf(), scanf(), etc.
#include <stdbool.h> // Includes the boolean data type (true/false)
#include <unistd.h> //  Includes POSIX operating system API functions, including signal handling functions.
#include <signal.h>  //  Includes signal handling functions like signal(), raise(), etc.

// Function definition for the signal handler
void alarm_handler(int sig) {
  //  This function will be called when the SIGALRM signal is received.
  //  'sig' is the signal number, but we don't use it in this case.
  printf("The bells have been rung!\n"); 
}

int main() {
  printf("This program will end once the bells have been rung\n");

  //  Register the signal handler function for the SIGALRM signal.
  //  This tells the system to call 'alarm_handler' when a SIGALRM is received.
  signal(SIGALRM, alarm_handler);

  //  Set an alarm for 5 seconds. 
  //  After 5 seconds, the system will send the SIGALRM signal to this process.
  alarm(5);

  printf("We slumber\n"); 

  //  Pause execution of the program until a signal is received.
  //  This will wait until the SIGALRM signal is delivered (after 5 seconds).
  pause();

  printf("We have awoken!\n");

  //  Return 0 to indicate successful program execution.
  return 0; 
}
