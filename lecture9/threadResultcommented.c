#include <stdio.h>  // Includes standard input/output library for functions like printf
#include <pthread.h> // Includes POSIX threads library for multithreading operations
#include <unistd.h>  // Includes standard Unix library for functions like sleep
#include <time.h>    // Includes time library for generating random numbers
#include <stdlib.h>  // Includes standard library for functions like rand and malloc

// Function to be executed by the threads
void* weirdFunction(void* arg) {

  // Allocate memory for an integer using malloc, this will hold the sleep duration
  int* sleepPointer = malloc(sizeof(int)); 

  // Generate a random number between 1 and 5 (inclusive) and store it in the allocated memory
  *sleepPointer = rand() % 5 + 1; 

  // Suspend execution of the thread for the randomly generated duration
  sleep(*sleepPointer); 

  // Return the pointer to the allocated memory containing the sleep duration
  return sleepPointer; 
}

// Main function of the program
int main() {

  // Seed the random number generator using the current time
  srand(time(0)); 

  // Create two thread objects
  pthread_t thread1;
  pthread_t thread2;

  // Pointers to store the return values from the threads
  void* res1;
  void* res2;

  // Create the first thread, executing weirdFunction with NULL as argument
  pthread_create(&thread1, NULL, weirdFunction, NULL); 
  // Create the second thread, also executing weirdFunction with NULL as argument
  pthread_create(&thread2, NULL, weirdFunction, NULL);

  // Wait for thread1 to finish and store its return value in res1
  pthread_join(thread1, &res1); 
  // Wait for thread2 to finish and store its return value in res2
  pthread_join(thread2, &res2);

  // Print the sleep duration of thread1
  printf("Thread 1 did a sleep for %d seconds\n", *(int*)res1);
  // Print the sleep duration of thread2
  printf("Thread 2 did a sleep for %d seconds\n", *(int*)res2);

  // Return 0 to indicate successful execution
  return 0;
}
