#include <stdio.h> // Includes standard input/output library for functions like printf
#include <unistd.h> // Includes POSIX operating system API for functions like sleep
#include <stdlib.h> // Includes general purpose functions like malloc, rand, and srand
#include <time.h> // Includes time functions like time
#include <pthread.h> // Includes POSIX threads library for multithreading

// Define a function called 'weirdFunction' that will be executed by a thread
void* weirdFunction(void* arg) {
  // Allocate memory on the heap for an integer using malloc. This memory will be used to store the sleep duration.
  int* sleepPointer = malloc(sizeof(int)); 

  // Generate a random number between 0 and 4 and store it in the allocated memory.
  *sleepPointer = rand() % 5; 

  // Pause the execution of the thread for the randomly generated duration.
  sleep(*sleepPointer);

  // Return the pointer to the allocated memory containing the sleep duration. This will be used to retrieve the sleep duration later.
  return sleepPointer;
}

int main() {
  // Seed the random number generator using the current time to ensure different random numbers are generated each time the program runs.
  srand(time(0));

  // Declare two thread objects to represent the two threads that will be created.
  pthread_t thread1;
  pthread_t thread2;

  // Declare two void pointers to store the return values (sleep durations) from the threads.
  void* res1;
  void* res2;

  // Create a new thread (thread1) that will execute the 'weirdFunction' function.
  // The second argument (NULL) specifies the default attributes for the thread.
  // The third argument (NULL) specifies that no arguments are passed to the function.
  pthread_create(&thread1, NULL, weirdFunction, NULL); 

  // Create a second thread (thread2) that will also execute the 'weirdFunction' function.
  pthread_create(&thread2, NULL, weirdFunction, NULL);

  // Wait for thread1 to finish executing and store its return value (sleep duration) in 'res1'.
  // The 'res1' pointer will be updated with the value returned by 'weirdFunction' (the pointer to the sleep duration).
  pthread_join(thread1, &res1); 

  // Wait for thread2 to finish executing and store its return value (sleep duration) in 'res2'.
  pthread_join(thread2, &res2);

  // Print the sleep durations for thread1 and thread2.
  printf("Thread 1 did a sleep for %d seconds\n", *(int*)res1); // Cast 'res1' to an integer pointer and dereference it to access the sleep duration.
  printf("Thread 2 did a sleep for %d seconds\n", *(int*)res2); // Cast 'res2' to an integer pointer and dereference it to access the sleep duration.

  // Return 0 to indicate successful program execution.
  return 0;
}
