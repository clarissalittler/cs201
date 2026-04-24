#include <pthread.h> // Include the header file for POSIX threads
#include <unistd.h> // Include the header file for standard Unix functions, including sleep()
#include <fcntl.h> // Include the header file for file control, not used in this example
#include <stdlib.h> // Include the header file for standard library functions, including rand() and srand()
#include <stdio.h> // Include the header file for standard input/output functions, including printf()
#include <time.h> // Include the header file for time functions, including time()

int ourCounter = 0; // Declare a global integer variable 'ourCounter' to store the shared counter value, initialized to 0.

// Define a thread function named 'threadCounter' that takes a void pointer as an argument
// and returns a void pointer.
void* threadCounter(void* arg){ 
  (void)arg;                    // This example does not use thread arguments.
  int temp = ourCounter; // Create a local copy 'temp' of the global counter variable 'ourCounter'.

  // Sleep for a random number of seconds between 0 and 2.
  // This simulates the time taken by each thread to perform its task.
  sleep(rand()%3); 

  // Increment the global counter 'ourCounter' by 1 using the local copy 'temp'.
  // This operation is not thread-safe because it's not atomic and can lead to race conditions.
  ourCounter = temp +1; 
  return NULL; // Return NULL to indicate the thread completed successfully.
}

// Define the main function where the program execution starts.
int main(){
  // Seed the random number generator with the current time to generate different random numbers each time.
  srand(time(0)); 

  // Declare an array 'threads' of 10 pthread_t structures to store thread identifiers.
  pthread_t threads[10]; 

  // Create 10 threads and assign each thread the 'threadCounter' function as its entry point.
  // The 'threadCounter' function is invoked with NULL as an argument, which is ignored in this case.
  // Each thread has a unique thread identifier stored in the 'threads' array.
  for(int i = 0; i < 10; i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  // Wait for each thread to finish before proceeding to the next step.
  // This ensures all threads have completed their execution.
  for(int i = 0; i < 10; i++){
    pthread_join(threads[i],NULL);
  }

  // Print the final value of the counter 'ourCounter'.
  // This is expected to be less than or equal to 10, as the program can be affected by race conditions.
  printf("What's the value of the counter? %d\n",ourCounter);
  
  return 0; // Return 0 to indicate successful program termination.
}

/*
 * Compare this racy example with threadmutexcommented.c for the
 * corrected version that protects the shared counter with a mutex.
 */
