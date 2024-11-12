#include <stdio.h>  // Includes standard input/output library for functions like printf.
#include <signal.h> // Includes signal handling library for functions like signal and sigint_handler.
#include <unistd.h> // Includes POSIX operating system API functions, including sleep.
#include <stdbool.h> // Includes boolean data type for true/false values.
#include <stdlib.h> // Includes standard library functions, including exit.

// Declares a global variable 'counter' as volatile. 
// 'volatile' tells the compiler that the variable's value can change unexpectedly, 
// preventing optimizations that might assume its value is constant.
volatile int counter = 0;

// Defines the function 'sigint_handler' that will be called when a SIGINT signal is received.
// SIGINT is the signal generated when the user presses Ctrl+C.
void sigint_handler(int sig){
  // Increments the counter variable to keep track of how many times Ctrl+C has been pressed.
  counter++;
  // Prints a message to the console, indicating that a SIGINT signal has been caught.
  // It also shows how many more times Ctrl+C needs to be pressed to terminate the program.
  printf("Caught a sigint: Press ctrl-c %d more times to exit\n",3-counter);
  // Checks if the counter has reached 3.
  if(counter >=3){
    // If the counter is 3 or more, it means Ctrl+C has been pressed 3 times.
    // This section prints a message and then exits the program with a status code of 0.
    printf("I've been banished!\n");
    exit(0);
  }
}

// The main function, the entry point of the program.
int main(){
  // Prints a message to the console, indicating that a Ctrl+C handler is set up.
  printf("We have a ctrl-c handler here!\n");
  // Registers the 'sigint_handler' function to handle SIGINT signals.
  // This means that whenever the user presses Ctrl+C, the 'sigint_handler' function will be executed.
  signal(SIGINT, sigint_handler);
  // Starts an infinite loop using 'while(true)'.
  while(true){
    // Prints "Boop boop" to the console every second.
    printf("Boop boop\n");
    // Pauses the program execution for 1 second.
    sleep(1);
  }
  
  // Returns 0 to indicate successful program execution, although this line will never be reached due to the infinite loop.
  return 0;
}
