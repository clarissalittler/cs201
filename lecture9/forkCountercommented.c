#include <stdio.h> //  Includes standard input/output library for functions like printf
#include <stdlib.h> // Includes standard library for functions like rand, srand, malloc, free, etc.
#include <unistd.h> // Includes POSIX operating system API for functions like fork, wait, etc.
#include <pthread.h> // Includes POSIX threads library for functions like pthread_create, pthread_join, etc.
#include <semaphore.h> // Includes POSIX semaphores library for functions like sem_init, sem_wait, sem_post, etc.
#include <sys/mman.h> // Includes memory mapping functions like mmap, munmap, etc.
#include <sys/wait.h> // Includes functions for waiting on child processes like wait, waitpid, etc.
#include <time.h> // Includes functions for working with time like time, srand, rand, etc.

// Define a structure to hold a counter and a semaphore
struct guardCounter {
  int counter; // The integer counter variable
  sem_t semaphore; // The semaphore used for synchronization
};

int main() {
  // Seed the random number generator with the current time
  srand(time(0));

  // Create a shared memory segment using mmap
  // Allocate memory for a struct guardCounter
  struct guardCounter* ourCounter = mmap(NULL, sizeof(struct guardCounter),
                                PROT_READ | PROT_WRITE, // Set permissions for reading and writing
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0); // Create shared anonymous memory

  // Initialize the semaphore associated with the shared memory
  // 1 for being shared between processes, 1 for starting value
  sem_init(&ourCounter->semaphore, 1, 1); 
  // Initialize the counter to 0
  ourCounter->counter = 0;

  // Fork a child process using fork()
  pid_t pid = fork();

  // Parent process logic
  if (pid != 0) {
    // Wait for the child process to finish
    wait(NULL); 

    // Print the final value of the counter
    printf("Survey says! %d\n", ourCounter->counter);

    // Destroy the semaphore
    sem_destroy(&ourCounter->semaphore);

    // Unmap the shared memory segment
    munmap(ourCounter, sizeof(struct guardCounter));

  } else { // Child process logic
    // Increment the counter 5 times using a semaphore for synchronization
    for (int i = 0; i < 5; i++) {
      // Acquire the semaphore
      sem_wait(&ourCounter->semaphore);

      // Read the current value of the counter
      int temp = ourCounter->counter;

      // Sleep for a random amount of time
      sleep(rand() % 3);

      // Increment the counter
      ourCounter->counter = temp + 1;

      // Release the semaphore
      sem_post(&ourCounter->semaphore);
    }
  }
  // Return 0 to indicate successful execution
  return 0;
}
