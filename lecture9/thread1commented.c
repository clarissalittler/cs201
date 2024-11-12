#include <stdio.h>  //  Includes standard input/output library for functions like printf.
#include <pthread.h> // Includes the POSIX threads library, which provides functions for creating and managing threads.
#include <unistd.h>   //  Includes standard Unix functions, such as sleep.
#include <time.h>    //  Includes time-related functions, not used in this example.
#include <stdlib.h>   //  Includes standard library functions, not used in this example.

// This function represents a simple thread task.
void* ourPrinter(void* arg) {
  // Cast the argument to a character pointer, as it represents a message string.
  char* msg = (char*) arg;

  // Print the message using printf.
  printf("Our thread says: %s", msg);

  // Return 0 to indicate successful thread execution.
  return 0;
}

// Main function where the program execution begins.
int main() {
  // Declare two pthread_t variables to represent the two threads we'll create.
  pthread_t thread1;
  pthread_t thread2;

  // Create two character pointers to hold the messages for each thread.
  char* msg1 = "Hi there I'm one thread\n";
  char* msg2 = "Hi there I'm a different thread\n";
  
  // Create the first thread using pthread_create.
  // - &thread1:  A pointer to the thread variable to store the thread ID.
  // - NULL:       Attributes of the thread (we're using the default here).
  // - ourPrinter:  The function to be executed by the thread.
  // - msg1:       The argument to pass to ourPrinter function.
  pthread_create(&thread1, NULL, ourPrinter, msg1);

  // Create the second thread similarly, using msg2 as the argument.
  pthread_create(&thread2, NULL, ourPrinter, msg2);

  // Wait for thread1 to finish using pthread_join.
  // - thread1: The thread to wait for.
  // - NULL:    Pointer to store the thread's return value (we're not using it here).
  pthread_join(thread1, NULL);

  // Wait for thread2 to finish similarly.
  pthread_join(thread2, NULL);

  // Return 0 to indicate successful program termination.
  return 0;
  
}
