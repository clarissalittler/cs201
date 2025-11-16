#include <stdio.h>        // Standard input and output functions
#include <pthread.h>      // POSIX threads library
#include <unistd.h>       // For sleep function
#include <time.h>         // For time-related functions
#include <stdlib.h>       // For dynamic memory allocation and rand functions

// Function executed by each thread
void* weirdFunction(void* arg) {
    // Allocate memory to store the sleep duration
    int* sleepPointer = malloc(sizeof(int));

    // Generate a random number between 1 and 5 and store it in the allocated memory
    *sleepPointer = rand() % 5 + 1;

    // Make the thread sleep for the generated number of seconds
    sleep(*sleepPointer);
    
    // Return the pointer to the allocated memory containing the sleep duration
    return sleepPointer;
}

int main() {
    // Seed the random number generator with the current time to ensure different results each run
    srand(time(0));

    // Declare two thread identifiers
    pthread_t thread1;
    pthread_t thread2;

    // Pointers to store the results returned by the threads
    void* res1;
    void* res2;

    // Create the first thread, executing weirdFunction with no arguments (NULL)
    if (pthread_create(&thread1, NULL, weirdFunction, NULL) != 0) {
        perror("Failed to create thread1");
        exit(EXIT_FAILURE);
    }

    // Create the second thread, executing weirdFunction with no arguments (NULL)
    if (pthread_create(&thread2, NULL, weirdFunction, NULL) != 0) {
        perror("Failed to create thread2");
        exit(EXIT_FAILURE);
    }

    // Wait for thread1 to finish and retrieve its result
    if (pthread_join(thread1, &res1) != 0) {
        perror("Failed to join thread1");
        exit(EXIT_FAILURE);
    }

    // Wait for thread2 to finish and retrieve its result
    if (pthread_join(thread2, &res2) != 0) {
        perror("Failed to join thread2");
        exit(EXIT_FAILURE);
    }

    // Cast the returned void pointers to integer pointers and dereference to get the sleep durations
    printf("Thread 1 did a sleep for %d seconds\n", *(int*)res1);
    printf("Thread 2 did a sleep for %d seconds\n", *(int*)res2);

    // Free the dynamically allocated memory for each thread's result
    free(res1);
    free(res2);

    return 0; // Indicate successful program termination
}
