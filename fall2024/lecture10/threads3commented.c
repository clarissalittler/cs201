#include <pthread.h>  // Include the header file for POSIX threads
#include <unistd.h>   // Include the header file for standard Unix functions (like sleep)
#include <fcntl.h>    // Include the header file for file control (not used in this example)
#include <stdlib.h>   // Include the header file for standard library functions (like malloc, rand)
#include <stdio.h>    // Include the header file for standard input/output (like printf, scanf)
#include <time.h>     // Include the header file for time functions (like time)

int ourCounter = 0;  // Declare a global variable to store the counter value

pthread_mutex_t mutex;  // Declare a mutex variable to protect the counter from race conditions

// Define a thread function that increments the counter by 1
void* threadCounter(void* arg) {
  // Acquire the mutex to protect the counter from concurrent access
  pthread_mutex_lock(&mutex); 

  // Store the current counter value in a temporary variable
  int temp = ourCounter; 

  // Sleep for a random amount of time (between 0 and 2 seconds)
  sleep(rand() % 3);

  // Increment the counter and update the global variable
  ourCounter = temp + 1; 

  // Release the mutex, allowing other threads to access the counter
  pthread_mutex_unlock(&mutex);

  // Return NULL to indicate successful thread execution
  return NULL;
}

// Define a thread function that allows the user to increment the counter by an arbitrary amount
void* wildCounter(void* arg) {
  // Prompt the user to enter a number
  printf("Enter a number, right quick: ");

  // Declare an integer variable to store the user's input
  int inc;

  // Read the user's input from standard input
  scanf("%d", &inc);

  // Acquire the mutex to protect the counter from concurrent access
  pthread_mutex_lock(&mutex);

  // Store the current counter value in a temporary variable
  int temp = ourCounter; 

  // Sleep for a random amount of time (between 0 and 2 seconds)
  sleep(rand() % 3);

  // Increment the counter by the user-provided value and update the global variable
  ourCounter = temp + inc;

  // Release the mutex, allowing other threads to access the counter
  pthread_mutex_unlock(&mutex);

  // Return NULL to indicate successful thread execution
  return NULL;
}

// Define the main function of the program
int main() {
  // Seed the random number generator with the current time for more random sleep values
  srand(time(0)); 

  // Declare an array of 10 thread structures
  pthread_t threads[10];

  // Initialize the mutex
  pthread_mutex_init(&mutex, NULL);

  // Create 10 threads
  for (int i = 0; i < 10; i++) {
    // For threads 0-4 and 6-9, call the threadCounter function
    if (i != 5) {
      pthread_create(&threads[i], NULL, threadCounter, NULL);
    } 
    // For thread 5, call the wildCounter function
    else {
      pthread_create(&threads[i], NULL, wildCounter, NULL);
    }
  }

  // Join the threads, waiting for them to complete execution
  for (int i = 0; i < 10; i++) {
    pthread_join(threads[i], NULL);
  }

  // Destroy the mutex
  pthread_mutex_destroy(&mutex);

  // Print the final value of the counter
  printf("What's the value of the counter? %d\n", ourCounter);

  // Return 0 to indicate successful program execution
  return 0;
}
