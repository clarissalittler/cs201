#include <stdio.h>      // Standard input/output library for printf
#include <stdlib.h>     // Standard library for functions like srand and rand
#include <time.h>       // Time library for seeding the random number generator
#include <pthread.h>    // POSIX threads library for threading functions
#include <unistd.h>     // Unix standard library for sleep function
#include <fcntl.h>      // File control options (not directly used in this code)
#include <sys/stat.h>   // System data types and structures (not directly used)
#include <semaphore.h>  // Semaphore library for semaphore functions

// Global counter that will be incremented by threads
int ourCounter = 0;

// Semaphore to control access to ourCounter
sem_t ourSem;

// Function that each thread will execute
void* threadCounter(void* arg){
    // Wait (decrement) the semaphore before entering critical section
    sem_wait(&ourSem);

    // Critical Section Start
    int temp = ourCounter;           // Read the current value of the counter
    sleep(rand() % 3);               // Simulate some work with random sleep (0-2 seconds)
    ourCounter = temp + 1;           // Increment the counter
    // Critical Section End

    // Post (increment) the semaphore to signal that the critical section is done
    sem_post(&ourSem);

    return NULL; // Thread exits
}

int main(){
    // Seed the random number generator with the current time
    srand(time(0));

    // Array to hold thread identifiers for 10 threads
    pthread_t threads[10];

    // Initialize the semaphore
    // The second argument '0' means the semaphore is shared between threads of the process
    // The third argument '1' sets the initial value of the semaphore to 1 (binary semaphore)
    if (sem_init(&ourSem, 0, 1) != 0) {
        perror("sem_init failed"); // Print error if semaphore initialization fails
        exit(EXIT_FAILURE);        // Exit the program with failure status
    }

    // Create 10 threads
    for(int i = 0; i < 10; i++){
        // pthread_create returns 0 on success
        if(pthread_create(&threads[i], NULL, threadCounter, NULL) != 0){
            perror("pthread_create failed"); // Print error if thread creation fails
            exit(EXIT_FAILURE);             // Exit the program with failure status
        }
    }

    // Wait for all 10 threads to finish execution
    for(int i = 0; i < 10; i++){
        if(pthread_join(threads[i], NULL) != 0){
            perror("pthread_join failed"); // Print error if joining thread fails
            exit(EXIT_FAILURE);            // Exit the program with failure status
        }
    }

    // Destroy the semaphore as it's no longer needed
    sem_destroy(&ourSem);

    // Print the final value of ourCounter
    printf("What's the value of this counter?? %d\n", ourCounter);

    return 0; // Exit the program successfully
}
