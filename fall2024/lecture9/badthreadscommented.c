#include <stdio.h>      // Standard I/O library for input and output functions
#include <stdlib.h>     // Standard library for general-purpose functions like malloc, rand, etc.
#include <time.h>       // Time library for functions related to time and date
#include <pthread.h>    // POSIX threads library for multithreading
#include <unistd.h>     // UNIX standard library for sleep function and other POSIX functions
#include <fcntl.h>      // File control options (not used in this example)
#include <sys/stat.h>   // Data returned by the stat() function (not used in this example)

// Global counter shared among all threads
int ourCounter = 0;

// Function that each thread will execute
void* threadCounter(void* arg){
    // Local copy of the global counter to reduce direct access (still prone to race conditions)
    int temp = ourCounter;

    // Sleep for a random duration between 0 and 2 seconds to simulate work and increase chance of race conditions
    sleep(rand() % 3);

    // Increment the copied value
    ourCounter = temp + 1;

    // Thread exits; return value is NULL since we don't need to pass any data back
    return NULL;
}

int main(){
    // Seed the random number generator with the current time to ensure different results on each run
    srand(time(0));

    // Array to hold thread identifiers for up to 10 threads
    pthread_t threads[10];

    // Loop to create 10 threads
    for(int i = 0; i < 10; i++){
        // Create a new thread that starts execution in the threadCounter function
        // &threads[i] stores the thread ID, NULL uses default thread attributes, and NULL passes no arguments
        if(pthread_create(&threads[i], NULL, threadCounter, NULL) != 0){
            // If pthread_create returns a non-zero value, an error occurred
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Loop to wait for all 10 threads to finish execution
    for(int i = 0; i < 10; i++){
        // Join each thread, blocking the main thread until the specified thread terminates
        if(pthread_join(threads[i], NULL) != 0){
            // If pthread_join returns a non-zero value, an error occurred
            perror("Failed to join thread");
            exit(EXIT_FAILURE);
        }
    }

    // After all threads have finished, print the final value of ourCounter
    printf("What's the value of this counter?? %d\n", ourCounter);

    // Return 0 to indicate successful execution
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// Global counter
int ourCounter = 0;

// Mutex to protect access to ourCounter
pthread_mutex_t counterMutex = PTHREAD_MUTEX_INITIALIZER;

void* threadCounter(void* arg){
    // Lock the mutex before accessing the shared variable
    pthread_mutex_lock(&counterMutex);

    int temp = ourCounter;
    sleep(rand() % 3); // Simulate work
    ourCounter = temp + 1;

    // Unlock the mutex after updating
    pthread_mutex_unlock(&counterMutex);

    return NULL;
}

int main(){
    srand(time(0));
    pthread_t threads[10];

    for(int i = 0; i < 10; i++){
        if(pthread_create(&threads[i], NULL, threadCounter, NULL) != 0){
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < 10; i++){
        if(pthread_join(threads[i], NULL) != 0){
            perror("Failed to join thread");
            exit(EXIT_FAILURE);
        }
    }

    printf("Final counter value: %d\n", ourCounter);

    // Destroy the mutex
    pthread_mutex_destroy(&counterMutex);

    return 0;
}
