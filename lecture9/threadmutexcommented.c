#include <stdio.h>      // Standard I/O library for input and output functions
#include <stdlib.h>     // Standard library for functions like rand() and srand()
#include <time.h>       // Time library for seeding the random number generator
#include <pthread.h>    // POSIX threads library for creating and managing threads
#include <unistd.h>     // UNIX standard library for sleep() function
#include <fcntl.h>      // File control options (not used in this program)
#include <sys/stat.h>   // Data returned by the stat() function (not used in this program)

// Global counter shared among threads
int ourCounter = 0;

// Mutex to protect access to the shared counter
pthread_mutex_t mutex;

/**
 * Function executed by each thread.
 * It safely increments the global counter by locking the mutex,
 * reading the current value, sleeping for a random time,
 * incrementing the counter, and then unlocking the mutex.
 *
 * @param arg Unused parameter for thread arguments
 * @return NULL
 */
void* threadCounter(void* arg){
    // Lock the mutex before accessing the shared counter to ensure exclusive access
    pthread_mutex_lock(&mutex);
    
    // Read the current value of the counter
    int temp = ourCounter;
    
    // Sleep for a random duration between 0 to 2 seconds
    sleep(rand() % 3);
    
    // Increment the counter by updating it with the temporary value plus one
    ourCounter = temp + 1;
    
    // Unlock the mutex to allow other threads to access the counter
    pthread_mutex_unlock(&mutex);
    
    // Thread execution complete
    return NULL;
}

int main(){
    // Seed the random number generator with the current time to ensure different sleep durations
    srand(time(0));
    
    // Array to hold thread identifiers for 10 threads
    pthread_t threads[10];

    // Initialize the mutex with default attributes (NULL as the second argument)
    // This prepares the mutex for use in locking and unlocking
    pthread_mutex_init(&mutex, NULL);

    // Create 10 threads, each running the threadCounter function
    for(int i = 0; i < 10; i++){
        // The first argument is a pointer to the pthread_t structure
        // The second argument is for thread attributes (NULL means default attributes)
        // The third argument is the function the thread will execute
        // The fourth argument is the argument to pass to the thread function (NULL in this case)
        if(pthread_create(&threads[i], NULL, threadCounter, NULL) != 0){
            // If thread creation fails, print an error message and exit
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish execution
    for(int i = 0; i < 10; i++){
        // The first argument is the thread to wait for
        // The second argument is where the thread's return value will be stored (NULL if not needed)
        if(pthread_join(threads[i], NULL) != 0){
            // If joining fails, print an error message and exit
            perror("Failed to join thread");
            exit(EXIT_FAILURE);
        }
    }

    // Destroy the mutex once it's no longer needed to free up resources
    pthread_mutex_destroy(&mutex);

    // Print the final value of the counter
    printf("What's the value of this counter? %d\n", ourCounter);
    
    // Return 0 to indicate successful execution
    return 0;
}
