#include <stdio.h> // Include the standard input/output library for functions like printf
#include <stdlib.h> // Include the standard library for functions like rand and malloc
#include <time.h> // Include the time library for functions like time, used for random number generation
#include <pthread.h> // Include the POSIX threads library for functions related to thread management
#include <unistd.h> // Include the POSIX operating system API for functions like sleep
#include <fcntl.h> // Include the file control library, not used in this example
#include <sys/stat.h> // Include the system stat library, not used in this example
#include <semaphore.h> // Include the semaphore library for functions related to semaphores

int ourCounter = 0; // Declare a global integer variable named ourCounter, initialized to 0

sem_t ourSem; // Declare a semaphore named ourSem

// Define a function named threadCounter that will be executed by each thread
void* threadCounter(void* arg) {
  // Wait on the semaphore ourSem. This will block the thread until the semaphore is available (has a value greater than 0).
  sem_wait(&ourSem); 
  // Store the current value of ourCounter in a temporary variable temp.
  int temp = ourCounter;
  // Sleep for a random number of seconds between 0 and 2.
  sleep(rand()%3); 
  // Increment ourCounter by 1.
  ourCounter = temp+1; 
  // Signal the semaphore ourSem. This will allow another thread to acquire the semaphore.
  sem_post(&ourSem); 
  return NULL; // Return NULL to indicate that the thread has completed successfully.
}

int main(){
  // Seed the random number generator using the current time.
  srand(time(0));
  // Declare an array of 10 pthread_t structures to represent the threads.
  pthread_t threads[10]; 

  // Initialize the semaphore ourSem with an initial value of 1.
  // The second argument (0) specifies that the semaphore is not shared between processes.
  sem_init(&ourSem,0,1); 

  // Create 10 threads that will execute the threadCounter function.
  for(int i = 0;i < 10; i++){
    // Create a new thread and assign its ID to the i-th element of the threads array.
    // The arguments to pthread_create are:
    //   - the address of the thread ID variable (threads[i])
    //   - NULL (attributes, not used)
    //   - the function to be executed by the thread (threadCounter)
    //   - NULL (arguments to the function, not used)
    pthread_create(&threads[i],NULL,threadCounter,NULL);
  }

  // Join the 10 threads to the main thread.
  for(int i =0; i < 10;i++){
    // Wait for the i-th thread to complete its execution.
    // The arguments to pthread_join are:
    //   - the thread ID (threads[i])
    //   - NULL (pointer to a location to store the thread's return value, not used)
    pthread_join(threads[i],NULL);
  }

  // Destroy the semaphore ourSem.
  sem_destroy(&ourSem);
  
  // Print the final value of ourCounter.
  printf("What's the value of this counter?? %d\n",ourCounter);
  return 0;
}
