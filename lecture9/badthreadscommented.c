#include <stdio.h>  // Include standard input/output library (for printf)
#include <stdlib.h> // Include standard library (for rand, srand)
#include <time.h>   // Include time library (for time)
#include <pthread.h> // Include POSIX threads library (for thread operations)
#include <unistd.h>  // Include POSIX operating system API (for sleep)
#include <fcntl.h>   // Include file control library (not used in this example)
#include <sys/stat.h> // Include system stat library (not used in this example)

int ourCounter = 0; // Declare a global variable to store the counter value

// Define a thread function that increments the counter
void* threadCounter(void* arg){ 
  // Create a temporary variable to store the current counter value
  int temp = ourCounter;
  // Sleep for a random amount of time between 0 and 2 seconds
  sleep(rand()%3);  
  // Increment the counter by 1 
  ourCounter = temp+1; 
  // Return NULL to indicate successful thread execution
  return NULL;
}

// Main function 
int main(){
  // Seed the random number generator with the current time
  srand(time(0));
  // Declare an array of 10 pthread_t objects to store thread IDs
  pthread_t threads[10];

  // Create 10 threads, each running the threadCounter function
  for(int i = 0;i < 10; i++){
    // Create a new thread with attributes specified by NULL
    pthread_create(&threads[i],NULL,threadCounter,NULL); 
  }

  // Wait for all 10 threads to complete before proceeding
  for(int i =0; i < 10;i++){
    // Join the thread with ID threads[i] to the main thread
    pthread_join(threads[i],NULL); 
  }

  // Print the final value of the counter
  printf("What's the value of this counter?? %d\n",ourCounter);
  // Return 0 to indicate successful program execution
  return 0;
}
