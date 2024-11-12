#include <stdio.h>  // Includes standard input/output library for functions like printf
#include <stdlib.h> // Includes standard library for functions like rand and srand
#include <unistd.h> // Includes POSIX operating system API for functions like sleep
#include <time.h>   // Includes time library for functions like time
#include <pthread.h> // Includes POSIX threads library for functions like pthread_create, pthread_join

// This function is executed by a separate thread
void* myFun(void* arg){
  
  // Generate a random integer between 1 and 3 (inclusive)
  int temp = rand()%3+1;
  
  // Suspend the thread for the specified number of seconds
  sleep(temp);
  
  // Print a message indicating the duration the thread slept
  printf("I slept for %d seconds\n",temp);

  // Return NULL to indicate successful completion of the thread function
  return NULL;
}

int main(){
  // Seed the random number generator with the current time to ensure different random sequences each run
  srand(time(0));
  
  // Declare two thread variables to represent the two threads
  pthread_t thread1;
  pthread_t thread2;

  // Create a new thread and assign it to the thread1 variable
  // pthread_create(&thread1, NULL, myFun, NULL);
  // Arguments:
  // - &thread1: Pointer to the thread variable where the thread ID will be stored
  // - NULL: Attributes for the thread (NULL means use default attributes)
  // - myFun: The function to be executed by the thread
  // - NULL: Arguments to be passed to the thread function (we're not passing any)
  pthread_create(&thread1,NULL,myFun,NULL);

  // Create a new thread and assign it to the thread2 variable
  pthread_create(&thread2,NULL,myFun,NULL);

  // Wait for thread1 to finish before continuing execution in the main thread
  // pthread_join(thread1, NULL);
  // Arguments:
  // - thread1: The thread to wait for
  // - NULL: A pointer to a location where the thread's exit status can be stored (we're not using it)
  pthread_join(thread1,NULL); 

  // Wait for thread2 to finish before continuing execution in the main thread
  pthread_join(thread2,NULL); 
  
  // Return 0 to indicate successful program execution
  return 0;
}
