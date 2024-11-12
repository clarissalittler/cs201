#include <stdio.h>  // Include standard input/output library (for printf)
#include <stdlib.h> // Include standard library (for rand and srand)
#include <time.h>   // Include time library (for seeding random number generator)
#include <pthread.h> // Include POSIX threads library (for thread creation and synchronization)
#include <unistd.h>  // Include POSIX operating system API (for sleep function)
#include <fcntl.h>   // Include file control library (not used in this example)
#include <sys/stat.h> // Include system statistics library (not used in this example)

// Declare a global variable to store the counter value
int ourCounter = 0; 

// Declare a mutex variable for thread synchronization
pthread_mutex_t mutex;

// Define the function that each thread will execute
void* threadCounter(void* arg){ 
  // Lock the mutex to prevent race conditions
  pthread_mutex_lock(&mutex); 

  // Create a temporary variable to store the current counter value
  int temp = ourCounter; 

  // Simulate some work by sleeping for a random amount of time
  sleep(rand()%3); // Sleep for 0 to 2 seconds

  // Increment the counter 
  ourCounter = temp+1; 

  // Unlock the mutex to allow other threads to access the shared counter
  pthread_mutex_unlock(&mutex); 

  // Return NULL to indicate successful thread execution
  return NULL; 
}

// Main function of the program
int main(){
  // Seed the random number generator with current time
  srand(time(0)); 

  // Declare an array of 10 thread handles
  pthread_t threads[10]; 

  // Initialize the mutex with default attributes (NULL)
  pthread_mutex_init(&mutex,NULL); 

  // Create 10 threads that will execute the threadCounter function
  for(int i=0;i<10;i++){
    pthread_create(&threads[i],NULL,threadCounter,NULL); 
  }

  // Wait for all threads to finish execution
  for(int i=0; i< 10; i++){
    pthread_join(threads[i],NULL); 
  }

  // Destroy the mutex to release its resources
  pthread_mutex_destroy(&mutex); 

  // Print the final value of the counter
  printf("What's the value of this counter? %d\n",ourCounter); 

  // Return 0 to indicate successful program execution
  return 0; 
}
